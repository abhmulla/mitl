/**
 * @file holdmode.cpp
 * @author Abdulelah Mulla
 */

#include "mode/holdmode.h"
#include <iostream>

ModeType HoldMode::get_type() const {
    return ModeType::Hold;
}

void HoldMode::enter() {
    std::cout << "[HoldMode] Entering hold - maintaining position" << std::endl;
}

void HoldMode::exit() {
    std::cout << "[HoldMode] Exiting hold mode" << std::endl;
}

void HoldMode::update() {
    // TODO: Maintain position
    // vehicle->hold_position();
}

bool HoldMode::is_complete() {
    // Hold mode doesn't complete automatically
    return false;
}
