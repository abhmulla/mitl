/**
 * @file takeoff.cpp
 * @author Abdulelah Mulla
 */

#include <iostream>

#include "mode/takeoff.h"
#include "morb.h"

Takeoff::Takeoff(Morb *morb) :
    _morb(morb)
{
    state_id = 0;
}

void Takeoff::on_activation() {
    /// Subscribe to the current position
    _morb->subscribe<Position>("vehicle_position", [this](const Position &pos){
        _current_pos = pos;
    });
    /// Get current position for takeoff starting point
    _target_pos.lat = _current_pos.lat;
    _target_pos.lon = _current_pos.lon;
    _target_pos.yaw = _current_pos.yaw;

    /// Target altitude, Hardcoded to 10m
    _takeoff_alt_amsl = _current_pos.alt + 10.0f;

    /// Initialize velocities to zero
    _target_pos.vx = 0;
    _target_pos.vy = 0;
    _target_pos.vz = -0.5;

    /// Update state
    _state = TakeoffState::CLIMBING;

    std::cout << "[Takeoff] Activated" << std::endl;
}

void Takeoff::on_active() {
    if (_state == TakeoffState::CLIMBING) {
        /// Publish position setpoint
        _morb->publish<Position>("position_setpoint", _target_pos);

        /// Check if we reached our target altitude
        float alt_error = std::abs(_current_pos.alt - _target_pos.alt);
        if (alt_error < ALTITUDE_THRESHOLD) {
            _state = TakeoffState::COMPLETE;
            std::cout << "[Takeoff] Complete" << std::endl;
        }
    } else if (_state == TakeoffState::COMPLETE) {
        /// Hold until mode changes
        _morb->publish<Position>("position_setpoint", _current_pos);
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