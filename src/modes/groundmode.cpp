/**
 * @file groundmode.cpp
 * @author Abdulelah Mulla
 */

#include "mode/groundmode.h"
#include <iostream>

ModeType GroundMode::get_type() const {
    return ModeType::Ground;
}

void GroundMode::enter() {
    std::cout << "[GroundMode] Vehicle on ground" << std::endl;
}

void GroundMode::exit() {
    std::cout << "[GroundMode] Leaving ground mode" << std::endl;
}

void GroundMode::update() {
    // Nothing to do while on ground
}

bool GroundMode::is_complete() {
    // Ground mode doesn't complete automatically
    return false;
}
