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

void Vehicle::publish_telem() {
    telem->publish_home(pos);
    telem->publish_sys_status(battery, true, true, true, true, true);
    telem->publish_position(pos, vel, hdg);
    telem->publish_position_velocity_ned(pos_vel);
    telem->publish_raw_gps(raw_gps, gps_info);
}

/// TODO: Implement
void Vehicle::takeoff() {
    pos.relative_altitude_m = 10;
}

/// TODO: Implement
void Vehicle::enter_hold() {

}