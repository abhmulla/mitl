/**
 * @file landmode.cpp
 * @author Abdulelah Mulla
 */

#include "mode/landmode.h"
#include <iostream>

ModeType LandMode::get_type() const {
    return ModeType::Land;
}

void LandMode::enter() {
    std::cout << "[LandMode] Starting landing sequence" << std::endl;
}

void LandMode::exit() {
    std::cout << "[LandMode] Exiting land mode" << std::endl;
}

void LandMode::update() {
    // TODO: Implement landing logic
    // vehicle->descend();
}

bool LandMode::is_complete() {
    // TODO: Check if landed
    // return vehicle->is_on_ground();
    return false;
}
