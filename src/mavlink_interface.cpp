/**
 * @file mavlink_interface.cpp
 * @author Abdulelah Mulla
 */

#include <chrono>
#include <cmath>
#include <iostream>

#include "mavlink_interface.h"
#include "log.h"

using namespace std::chrono_literals;

MavlinkInterface::MavlinkInterface(
    Morb* morb, std::string url,
    mavsdk::ComponentType type):
    _connection_url(std::move(url)),
    _config(type),
    _mavsdk(_config),
    _mission_future(_mission_prom.get_future()),
    _morb(morb)
    {
        mitl_log << "[MavlinkInterface] Initialized MavlinkInterface" << std::endl;
    }

MavlinkInterface::~MavlinkInterface() {
    stop();
    mitl_log << "[MavlinkInterface] Destroyed MavlinkInterface" << std::endl;
}

/// Initialize Drone connection via UDP Port
bool MavlinkInterface::setup_connection() {
    std::cout << "Connecting to " << _connection_url <<'\n';
    const mavsdk::ConnectionResult res = _mavsdk.add_any_connection(_connection_url);
    if (res != mavsdk::ConnectionResult::Success) {
        std::cerr << "Connection failed: " << res <<'\n';
        return false;
    }
    _server = _mavsdk.server_component();
    /// HARDCODED FOR NOW
    _server->set_system_status(MAV_STATE_ACTIVE);
    /// Wait for system to be discovered
    std::cout << "Waiting for drone to connect..." << '\n';
    std::promise<std::shared_ptr<mavsdk::System>> prom;
    std::future<std::shared_ptr<mavsdk::System>> fut = prom.get_future();

    // Add new temporary callback that gets called upon system add:
    // (Callback implemented via lambda)

    const mavsdk::Mavsdk::NewSystemHandle handle = _mavsdk.subscribe_on_new_system([this, &prom]() {
        auto systems = _mavsdk.systems();
        std::cout << "Number of systems detected: " << systems.size() << '\n';

        if (!systems.empty()) {
            _system = systems.at(0);
            prom.set_value(_system);
        } else {
            std::cout << "No systems found." << '\n';
            prom.set_value(nullptr);
        } 
    });

    // Wait for system to be configured:

    _system = fut.get();
    if (!_system) {
        std::cerr << "Failed to connect to the drone." << '\n';
        return false;
    }

    // Remove system callback:

    _mavsdk.unsubscribe_on_new_system(handle);

    if (!_system->is_connected()) {
        std::cerr << "System is not connected!" << '\n';
        return false;
    }

    return (_server != nullptr && _system != nullptr);
}

void MavlinkInterface::setup_params() {
    /// PX4-style and custom params
    _param->provide_param_int("MIS_TAKEOFF_ALT", 0);
    _param->provide_param_int("MY_PARAM", 1);
}

void MavlinkInterface::on_takeoff(mavsdk::ActionServer::Result result, bool in_prog) {
    if (result == mavsdk::ActionServer::Result::Success) {
        _manager->activate_takeoff();
    }
}

void MavlinkInterface::on_land(mavsdk::ActionServer::Result result, bool in_prog) {
    if (result == mavsdk::ActionServer::Result::Success) {
        _manager->activate_land();
    }
}

void MavlinkInterface::on_mode_change(mavsdk::ActionServer::Result res, mavsdk::ActionServer::FlightMode mode) {
    _manager->change_mode(mode);
}

void MavlinkInterface::vehicle_loop() {
    while (_running) {
        /// 1hz rate
        _vehicle->publish_telem();
        std::this_thread::sleep_for(1s);
    }
}

void MavlinkInterface::on_arm_disarm(mavsdk::ActionServer::Result result, mavsdk::ActionServer::ArmDisarm arm_disarm) {
    if (result == mavsdk::ActionServer::Result::Success) {
        if(arm_disarm.arm) {
            mitl_log << "[MavlinkInterface] Arming requested" << std::endl;
            _vehicle->arm();
            /// Update state to indicate we're armed
            _armed.store(true);
        } else {
            mitl_log << "[MavlinkInterface] Disarming requested" << std::endl;
            _vehicle->disarm();
            _armed.store(false);
        }
    } else {
        mitl_log << "[MavlinkInterface] Arm/Disarm request failed: " << result << std::endl;
        _armed.store(false);
    }
}

void MavlinkInterface::setup_actions() {
    _action->set_allowable_flight_modes({true, true, true});
    _action->set_armable(true, true);
    _action->set_disarmable(true, true);
    _action->set_armed_state(_armed);
    _action->set_allow_takeoff(true);

    _action->subscribe_takeoff([this](auto r, bool b){ this->on_takeoff(r,b); });
    _action->subscribe_land([this](auto r, bool b){ this->on_land(r,b); });
    _action->subscribe_flight_mode_change([this](mavsdk::ActionServer::Result result, mavsdk::ActionServer::FlightMode mode) {
                  on_mode_change(result, mode);
              });
    _action->subscribe_arm_disarm([this](mavsdk::ActionServer::Result result, mavsdk::ActionServer::ArmDisarm arm_disarm){
        this->on_arm_disarm(result,arm_disarm); });
}

void MavlinkInterface::on_incoming_mission(mavsdk::MissionRawServer::Result res,
                                           mavsdk::MissionRawServer::MissionPlan plan) {
    if (res != mavsdk::MissionRawServer::Result::Success) {
        std::cerr << "Mission upload failed: " << '\n';
    }
    mitl_log << "Received Uploaded Mission\n" << plan << std::endl;
}

void MavlinkInterface::setup_mission_server() {
    mitl_log << "MissionRawServer created\n";
    _mission_handle = _mission->subscribe_incoming_mission(
        [this](auto res, auto plan){ this->on_incoming_mission(res, std::move(plan)); });

    _mission->subscribe_current_item_changed(
        [](mavsdk::MissionRawServer::MissionItem item) {
            mitl_log << "Current item changed: " << item << std::endl;
        });

    _mission->subscribe_clear_all([](uint32_t){
        mitl_log << "Clear All Mission!\n";
    });
}

bool MavlinkInterface::start() {
    if (!setup_connection()) return false;
    _vehicle = std::make_unique<Vehicle>(_server, _system, _morb);
    _action = std::make_unique<mavsdk::ActionServer>(_server);
    _manager = std::make_unique<ModeManager>(*_vehicle, *_action, _morb);
    _param = std::make_unique<mavsdk::ParamServer>(_server);
    _mission = std::make_unique<mavsdk::MissionRawServer>(_server);
    mitl_log <<"Setting up params"<<std::endl;
    setup_params();
    mitl_log <<"Setting up action"<<std::endl;
    setup_actions();
    mitl_log <<"Setting up mission"<<std::endl;
    setup_mission_server();
    mitl_log <<"Setting up modes"<<std::endl;
    _manager->initialize_modes();
    _running = true;
    return true;
}

void MavlinkInterface::run() {
    _manager->start();
    _vehicle_thread = std::thread(&MavlinkInterface::vehicle_loop, this);
}

void MavlinkInterface::stop() {
    if (_manager) {
        _manager->stop();
    }
    _running = false;
    if (_vehicle_thread.joinable()) {
        _vehicle_thread.join();
    }
}