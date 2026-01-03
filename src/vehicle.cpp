/**
 * @file vehicle.cpp
 * @author Abdulelah Mulla
 */

#include <iostream>

#include "vehicle.h"
#include "log.h"

Vehicle::Vehicle(std::shared_ptr<mavsdk::ServerComponent> server, std::shared_ptr<mavsdk::System> system, Morb* morb):
    _server(server), 
    _system(system),
    _morb(morb) 
    {
        _telem = std::make_unique<mavsdk::TelemetryServer>(server);
        _mavdirect = std::make_unique<mavsdk::MavlinkDirect>(system);
        MITL_LOG::initialize().program_log("[Vehicle] Initialzed Vehicle");
    }

Vehicle::~Vehicle() {
    MITL_LOG::initialize().program_log("[Vehicle] Destroyed Vehicle");
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
    MITL_LOG::initialize().program_log("[Vehicle] Arming requested");
    _arming_in_progress = true;
    // TODO: Send actual arm command via MAVSDK
    // For testing, simulate instant arming
    _armed = true;
    _arming_in_progress = false;
}

void Vehicle::disarm() {
    MITL_LOG::initialize().program_log("[Vehicle] Disarming requested");
    /// TODO: disarm the vehicle
    _armed = false;
}

void Vehicle::publish_telem() {
    _telem->publish_home(_pos);
    _telem->publish_sys_status(_battery, true, true, true, true, true);
    _telem->publish_position(_pos, _vel, _hdg);
    _telem->publish_position_velocity_ned(_pos_vel);
    _telem->publish_raw_gps(_raw_gps, _gps_info);
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