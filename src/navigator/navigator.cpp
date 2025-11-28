/**
 * @file navigator.cpp
 * @author Abdulelah Mulla
 */

#include <iostream>

#include "navigator/navigator.h"
#include "morb.h"
#include "position.h"

Navigator::Navigator(Morb* morb): 
    _morb(morb), 
    _takeoff(_morb)
{
    /// Initialize mode array
    _modes[0] = &_takeoff;
    _modes[1] = &_hold;
    _modes[2] = &_land;

    /// Subscribe
    _morb->subscribe<Position>("vehicle_position", [this](const Position &pos) {
        update_position(pos);
    });
}

void Navigator::run() {
    /// Iterate through mode list and set appropriately
    for (int i = 0; i < MODE_ARRAY_SIZE; i++) {
        if(_modes[i]) {
            _modes[i]->run(_curr_mode == _modes[i]);
        }
    }
    /// Check if the current mode is complete
    if (_curr_mode->is_complete()) {
        /// Signal to ModeManager we're done here
        if (_curr_mode->state_id == 0) {
            _morb->publish<std::string>("mode_complete", "active");
        } else if (_curr_mode->state_id == 1) {
            _morb->publish<std::string>("mode_complete", "takeoff");
        } else if (_curr_mode->state_id == 2) {
             _morb->publish<std::string>("mode_complete", "hold");
        } else if (_curr_mode->state_id == 3) {
             _morb->publish<std::string>("mode_complete", "land");
        }
    }
}

void Navigator::update_position(const Position &pos) {
    _pos = pos;
}

void Navigator::set_mode(mavsdk::ActionServer::FlightMode mode) {
    if (mode == mavsdk::ActionServer::FlightMode::Ready) {
        _curr_mode = &_active;
    } else if (mode == mavsdk::ActionServer::FlightMode::Takeoff) {
        _curr_mode = &_takeoff;
    } else if (mode == mavsdk::ActionServer::FlightMode::Hold) {
        _curr_mode = &_hold;
    } else if (mode == mavsdk::ActionServer::FlightMode::Land) {
        _curr_mode = &_land;
    }
}