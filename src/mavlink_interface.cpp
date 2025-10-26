/**
 * @file mavlink_interface.cpp
 * @author Abdulelah Mulla
 */

#include "mavlink_interface.h"
#include <chrono>
#include <cmath>
#include <iostream>

using namespace std::chrono_literals;

/// Initialize Drone connection via UDP Port
bool MavlinkInterface::setup_connection() {
    std::cout << "Connecting to " << connection_url <<'\n';
    const mavsdk::ConnectionResult res = mavsdk.add_any_connection(connection_url);
    if (res != mavsdk::ConnectionResult::Success) {
        std::cerr << "Connection failed: " << res <<'\n';
        return false;
    }
    server = mavsdk.server_component();
    /// Wait for system to be discovered
    std::cout << "Waiting for drone to connect..." << '\n';
    std::promise<std::shared_ptr<mavsdk::System>> prom;
    std::future<std::shared_ptr<mavsdk::System>> fut = prom.get_future();

    // Add new temporary callback that gets called upon system add:
    // (Callback implemented via lambda)

    const mavsdk::Mavsdk::NewSystemHandle handle = mavsdk.subscribe_on_new_system([this, &prom]() {
        auto systems = mavsdk.systems();
        std::cout << "Number of systems detected: " << systems.size() << '\n';

        if (!systems.empty()) {
            system = systems.at(0);
            prom.set_value(system);
        } else {
            std::cout << "No systems found." << '\n';
            prom.set_value(nullptr);
        } 
    });

    // Wait for system to be configured:

    system = fut.get();
    if (!system) {
        std::cerr << "Failed to connect to the drone." << '\n';
        return false;
    }

    // Remove system callback:

    mavsdk.unsubscribe_on_new_system(handle);

    if (!system->is_connected()) {
        std::cerr << "System is not connected!" << '\n';
        return false;
    }

    return (server != nullptr && system != nullptr);
}

void MavlinkInterface::setup_params() {
    /// PX4-style and custom params
    param->provide_param_int("MIS_TAKEOFF_ALT", 0);
    param->provide_param_int("MY_PARAM", 1);
}

/// TODO: Implement
void MavlinkInterface::on_takeoff(mavsdk::ActionServer::Result result, bool in_prog) {
    if (result == mavsdk::ActionServer::Result::Success && in_prog) {
        //pos.relative_altitude_m = 10.f;
        manager->activate_takeoff();
    }
}

/// TODO: Implement
void MavlinkInterface::on_land(mavsdk::ActionServer::Result result, bool in_prog) {
    if (result == mavsdk::ActionServer::Result::Success && in_prog) {
        //pos.relative_altitude_m = 0.f;
        manager->activate_land();
    }
}

void MavlinkInterface::vehicle_loop() {
    while (running) {
        /// 1hz rate
        vehicle->publish_telem();
        std::this_thread::sleep_for(1s);
    }
}

/// TODO: Implement
// void MavlinkInterface::on_arm_disarm(mavsdk::ActionServer::Result result, bool in_prog) {
//     manager->arm();
// }

void MavlinkInterface::setup_actions() {
    action->set_allowable_flight_modes({true, true, true});
    action->set_allow_takeoff(true);
    action->set_armable(true, true);

    action->subscribe_takeoff([this](auto r, bool b){ this->on_takeoff(r,b); });
    action->subscribe_land([this](auto r, bool b){ this->on_land(r,b); });
    //action->subscribe_arm_disarm([this](auto r, bool b){this->on_arm_disarm(r,b); });
}

void MavlinkInterface::on_incoming_mission(mavsdk::MissionRawServer::Result res,
                                           mavsdk::MissionRawServer::MissionPlan plan) {
    if (res != mavsdk::MissionRawServer::Result::Success) {
        std::cerr << "Mission upload failed: " << '\n';
    }
    std::cout << "Received Uploaded Mission!\n" << plan << std::endl;
}

void MavlinkInterface::setup_mission_server() {
    std::cout << "MissionRawServer created\n";
    mission_handle = mission->subscribe_incoming_mission(
        [this](auto res, auto plan){ this->on_incoming_mission(res, std::move(plan)); });

    mission->subscribe_current_item_changed(
        [](mavsdk::MissionRawServer::MissionItem item) {
            std::cout << "Current item changed: " << item << std::endl;
        });

    mission->subscribe_clear_all([](uint32_t){
        std::cout << "Clear All Mission!\n";
    });
}

bool MavlinkInterface::start() {
    if (!setup_connection()) return false;
    vehicle = std::make_unique<Vehicle>(server, system);
    manager = std::make_unique<ModeManager>(*vehicle);
    action = std::make_unique<mavsdk::ActionServer>(server);
    param = std::make_unique<mavsdk::ParamServer>(server);
    mission = std::make_unique<mavsdk::MissionRawServer>(server);
    std::cout<<"Setting up params"<<std::endl;
    setup_params();
    std::cout<<"Setting up action"<<std::endl;
    setup_actions();
    std::cout<<"Setting up mission"<<std::endl;
    setup_mission_server();
    std::cout<<"Setting up modes"<<std::endl;
    manager->initialize_modes();
    running = true;
    return true;
}

void MavlinkInterface::run() {
    manager->start();
    vehicle_thread = std::thread(&MavlinkInterface::vehicle_loop, this);
}

void MavlinkInterface::stop() {
    manager->stop();
    running = false;
    if (vehicle_thread.joinable()) {
          vehicle_thread.join();
    }
}