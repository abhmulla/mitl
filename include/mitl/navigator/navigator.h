/**
 * @file navigator.h
 * @author Abdulelah Mulla
 * @brief Header file for the Navigator object
 * @version 0.1
 * @date 2025-11-20
 */

#pragma once

#include "mode/mode.h"
#include "mode/takeoff.h"
#include "mode/hold.h"
#include "mode/land.h"
#include "morb.h"
#include "position.h"

#include <mavsdk/mavsdk.h>

/**
 * Number of modes we are using
 */
#define MODE_ARRAY_SIZE 3

/**
 * @brief Provides waypoint for navigation based on 
 * the current mode of the vehicle.
 */
class Navigator {
private:
    Mode *_curr_mode{nullptr};  // pointer to the current mode
    Mode *_modes[MODE_ARRAY_SIZE] {}; // pointer to an array of modes

    /// Our modes
    Takeoff _takeoff;
    Hold _hold;
    Land _land;

    /// Message bus
    Morb* _morb;

    /// Position stuff
    Position _pos{};
    bool _pos_updated{false};

public:
    /// Constructor
    Navigator(Morb* morb);

    /// Disable copy constructor and assignment operator
    Navigator(const Navigator&) = delete;
    Navigator& operator=(const Navigator&) = delete;

    void run();

    void update_position(const Position &pos);
};