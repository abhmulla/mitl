/**
 * @file takeoffmode.cpp
 * @author Abdulelah Mulla
 */

#include "mode/takeoffmode.h"
#include <iostream>

ModeType TakeOffMode::get_type() const {
    return ModeType::Takeoff;
}

void TakeOffMode::enter() {
    std::cout << "[TakeOffMode] Entering takeoff mode" << std::endl;
    // TODO: Implement actual takeoff logic
    // vehicle->set_target_altitude(10.0f);
}

void TakeOffMode::exit() {
    std::cout << "[TakeOffMode] Exiting takeoff mode" << std::endl;
    // TODO: Implement cleanup
}

void TakeOffMode::update() {
    // TODO: Implement actual takeoff control logic
    // For now, just run for a bit then complete

    // Example: Check if we've reached target altitude
    // if (vehicle->get_altitude() >= target_altitude) {
    //     complete = true;
    // }
}

bool TakeOffMode::is_complete() {
    // TODO: Implement actual completion check
    // For now, never complete (will stay in takeoff until manual mode change)
    return false;
}
