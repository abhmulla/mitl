/**
 * @file vehicle.cpp
 * @author Abdulelah Mulla
 */

#include "vehicle/vehicle.h"
#include <iostream>

Vehicle::Vehicle(std::shared_ptr<mavsdk::ServerComponent> server, std::shared_ptr<mavsdk::System> system, Morb* morb):
    _server(server), 
    _system(system),
    _morb(morb) 
    {
        _telem = std::make_unique<mavsdk::TelemetryServer>(server);
        _mavdirect = std::make_unique<mavsdk::MavlinkDirect>(system);
    }

/// TODO: Implement
bool Vehicle::is_armed() {
    return _armed;
}

/// TODO: Implement
bool Vehicle::is_arming() {
    return _arming_in_progress;
}

/// TODO: Implement
void Vehicle::arm() {
    std::cout << "[Vehicle] Arming requested" << std::endl;
    _arming_in_progress = true;
    // TODO: Send actual arm command via MAVSDK
    // For testing, simulate instant arming
    _armed = true;
    _arming_in_progress = false;
}

void Vehicle::disarm() {
    std::cout << "[Vehicle] Disarming requested" << std::endl;
    /// TODO: disarm the vehicle
    _armed = false;
}

void Vehicle::publish_telem() {
    _telem->publish_home(_pos);
    _telem->publish_sys_status(_battery, true, true, true, true, true);
    _telem->publish_position(_pos, _vel, _hdg);
    _telem->publish_position_velocity_ned(_pos_vel);
    _telem->publish_raw_gps(_raw_gps, _gps_info);

    /// Publish landed state
    mavsdk::TelemetryServer::LandedState landed_state;
    switch(_curr_mode) {
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
    _telem->publish_extended_sys_state(mavsdk::TelemetryServer::VtolState::Mc, landed_state);
}

/// TODO: Implement
void Vehicle::takeoff() {
    _pos.relative_altitude_m = 10;
}

/// TODO: Implement
void Vehicle::enter_hold() {

}

void Vehicle::set_mode(mavsdk::ActionServer::FlightMode mode) {
    _curr_mode = mode;
}