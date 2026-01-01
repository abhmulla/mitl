/**
 * @file takeoff.cpp
 * @author Abdulelah Mulla
 */

#include <iostream>

#include "mode/takeoff.h"
#include "navigator/navigator.h"
#include "morb.h"
#include "log.h"

Takeoff::Takeoff(Morb *morb, Navigator *navigator) :
    _morb(morb),
    _navigator(navigator)
{
    state_id = 1;
    mitl_log << "[Takeoff] Initialized Takeoff" << std::endl;
}

Takeoff::~Takeoff() {
    mitl_log << "[Takeoff] Destroyed Takeoff" << std::endl;
}

void Takeoff::on_activation() {
    /// Get position from navigator
    PosSet *pos =  _navigator->get_position();

    /// Get current position for takeoff starting point
    pos->target.lat = pos->current.lat;
    pos->target.lon = pos->current.lon;
    pos->target.yaw = pos->current.yaw;

    /// Target altitude, Hardcoded to 10m
    _takeoff_alt_amsl = pos->current.alt + 10.0f;
    pos->target.alt = _takeoff_alt_amsl;

    /// Initialize velocities to zero
    pos->target.vx = 0;
    pos->target.vy = 0;
    pos->target.vz = -0.5;

    /// Update state
    _state = TakeoffState::CLIMBING;

    mitl_log << "[Takeoff] Activated" << std::endl;
}

void Takeoff::on_active() {
    /// Get position from navigator
    PosSet *pos = _navigator->get_position();

    if (_state == TakeoffState::CLIMBING) {
        /// Notify navigator to publish the position setpoint
        _navigator->notify_position_updated();

        /// Check if we reached our target altitude
        float alt_error = std::abs(pos->current.alt - pos->target.alt);
        if (alt_error < ALTITUDE_THRESHOLD) {
            _state = TakeoffState::COMPLETE;
            mitl_log << "[Takeoff] Complete" << std::endl;
        }
    } else if (_state == TakeoffState::COMPLETE) {
        /// Hold current position - set target to current
        pos->target = pos->current;
        _navigator->notify_position_updated();
    }
}

void Takeoff::on_inactivation() {
    _state = TakeoffState::INIT;
}

void Takeoff::on_inactive() {
}

bool Takeoff::is_complete() const {
    return _state == TakeoffState::COMPLETE;
}