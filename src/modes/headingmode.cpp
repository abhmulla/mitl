/**
 * @file headingmode.cpp
 * @author Abdulelah Mulla
 */

#include "mode/headingmode.h"
#include <iostream>

ModeType HeadingMode::get_type() const {
    return ModeType::Heading;
}

void HeadingMode::enter() {
    std::cout << "[HeadingMode] Flying to waypoint" << std::endl;
}

void HeadingMode::exit() {
    std::cout << "[HeadingMode] Exiting heading mode" << std::endl;
}

void HeadingMode::update() {
    // TODO: Implement waypoint navigation
    // vehicle->fly_to_waypoint(target_lat, target_lon);
}

bool HeadingMode::is_complete() {
    // TODO: Check if reached waypoint
    // return vehicle->distance_to_waypoint() < threshold;
    return false;
}
