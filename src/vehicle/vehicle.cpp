/**
 * @file vehicle.cpp
 * @author Abdulelah Mulla
 */

#include "vehicle/vehicle.h"
#include <iostream>

/// TODO: Implement
bool Vehicle::is_armed() {
    return armed;
}

/// TODO: Implement
bool Vehicle::is_arming() {
    return arming_in_progress;
}

/// TODO: Implement
void Vehicle::arm() {
    std::cout << "[Vehicle] Arming requested" << std::endl;
    arming_in_progress = true;
    // TODO: Send actual arm command via MAVSDK
    // For testing, simulate instant arming
    armed = true;
    arming_in_progress = false;
}

void Vehicle::disarm() {
    std::cout << "[Vehicle] Disarming requested" << std::endl;
    /// TODO: disarm the vehicle
    armed = false;
}

void Vehicle::publish_telem() {
    telem->publish_home(pos);
    telem->publish_sys_status(battery, true, true, true, true, true);
    telem->publish_position(pos, vel, hdg);
    telem->publish_position_velocity_ned(pos_vel);
    telem->publish_raw_gps(raw_gps, gps_info);

    /// Publish landed state
    mavsdk::TelemetryServer::LandedState landed_state;
    switch(curr_mode) {
        case mavsdk::ActionServer::FlightMode::Ready:
            landed_state = mavsdk::TelemetryServer::LandedState::OnGround;
            break;
        case mavsdk::ActionServer::FlightMode::Takeoff:
            landed_state = mavsdk::TelemetryServer::LandedState::TakingOff;
            break;
        case mavsdk::ActionServer::FlightMode::Land:
            landed_state = mavsdk::TelemetryServer::LandedState::Landing;
            break;
        case mavsdk::ActionServer::FlightMode::Hold:
        case mavsdk::ActionServer::FlightMode::Mission:
            landed_state = mavsdk::TelemetryServer::LandedState::InAir;
            break;
        default:
            landed_state = mavsdk::TelemetryServer::LandedState::Unknown;
    }
    telem->publish_extended_sys_state(mavsdk::TelemetryServer::VtolState::Mc, landed_state);
}

/// TODO: Implement
void Vehicle::takeoff() {
    pos.relative_altitude_m = 10;
}

/// TODO: Implement
void Vehicle::enter_hold() {

}

void Vehicle::set_mode(mavsdk::ActionServer::FlightMode mode) {
    curr_mode = mode;
}