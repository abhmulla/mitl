/**
 * @file mavlink_interface.cpp
 * @author Abdulelah Mulla
 */

#include "mavlink_interface.h"
#include <chrono>
#include <cmath>
#include <iostream>

using namespace std::chrono_literals;

MavlinkInterface::MavlinkInterface(std::string url, mavsdk::ComponentType type)
    : connection_url(std::move(url)),
      config(type),
      mavsdk(config),
      mission_future(mission_prom.get_future()) {}

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

void MavlinkInterface::setup_plugins() {
    param = std::make_unique<mavsdk::ParamServer>(server);
    telem = std::make_unique<mavsdk::TelemetryServer>(server);
    action = std::make_unique<mavsdk::ActionServer>(server);
    mission = std::make_unique<mavsdk::MissionRawServer>(server);
}

void MavlinkInterface::setup_params() {
    /// PX4-style and custom params
    param->provide_param_int("MIS_TAKEOFF_ALT", 0);
    param->provide_param_int("MY_PARAM", 1);
}

/// TODO: Implement
void MavlinkInterface::on_takeoff(mavsdk::ActionServer::Result result, bool takeoff) {
    if (result == mavsdk::ActionServer::Result::Success && takeoff) {
        pos.relative_altitude_m = 10.f;
    }
}

/// TODO: Implement
void MavlinkInterface::on_land(mavsdk::ActionServer::Result result, bool land) {
    if (result == mavsdk::ActionServer::Result::Success && land) {
        pos.relative_altitude_m = 0.f;
    }
}

void MavlinkInterface::setup_actions() {
    action->set_allowable_flight_modes({true, true, true});
    action->set_allow_takeoff(true);
    action->set_armable(true, true);

    action->subscribe_takeoff([this](auto r, bool b){ this->on_takeoff(r,b); });
    action->subscribe_land([this](auto r, bool b){ this->on_land(r,b); });
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
    setup_plugins();
    setup_params();
    /// seed some telemetry and publish home
    telem->publish_home(pos);
    setup_actions();
    setup_mission_server();

    running = true;
    return true;
}

void MavlinkInterface::run() {
    while (running) {
        telem->publish_home(pos);
        telem->publish_sys_status(battery, true, true, true, true, true);
        telem->publish_position(pos, vel, hdg);
        telem->publish_position_velocity_ned(pos_vel);
        telem->publish_raw_gps(raw_gps, gps_info);
        std::this_thread::sleep_for(1s);
    }
}

void MavlinkInterface::stop() {
    running = false;
}