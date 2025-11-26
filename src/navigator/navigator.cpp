/**
 * @file navigator.cpp
 * @author Abdulelah Mulla
 */

#include "navigator/navigator.h"
#include "morb.h"
#include "position.h"

Navigator::Navigator(Morb* morb): _morb(morb) {
    /// Initialize mode array
    _modes[0] = &_takeoff;
    _modes[1] = &_hold;
    _modes[2] = &_land;

    /// Subscribe
    _morb->subscribe<Position>("position", [this](const Position &pos) {
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
    /// Publish updates
    if (_pos_updated) {
        _morb->publish<Position>("position", _pos);
        _pos_updated = false;
    }
}

void Navigator::update_position(const Position &pos) {
    _pos = pos;
    _pos_updated = true;
}