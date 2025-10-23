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
    server= mavsdk.server_component();
    return (server != nullptr);
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
    vehicle = std::make_unique<Vehicle>(server);
    manager = std::make_unique<ModeManager>(*vehicle);
    action = std::make_unique<mavsdk::ActionServer>(server);
    param = std::make_unique<mavsdk::ParamServer>(server);
    mission = std::make_unique<mavsdk::MissionRawServer>(server);
    setup_params();
    setup_actions();
    setup_mission_server();
    manager->initialize_modes();
    running = true;
    return true;
}

void MavlinkInterface::run() {
    manager->start();
    while (running) {
        vehicle->publish_telem();
        std::this_thread::sleep_for(1s);
    }
}

void MavlinkInterface::stop() {
    manager->stop();
    running = false;
}