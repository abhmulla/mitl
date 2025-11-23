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

#include <mavsdk/mavsdk.h>

/**
 * Number of modes we are using
 */
#define MODE_ARRAY_SIZE 2

/**
 * @brief Provides waypoint for navigation based on 
 * the current mode of the vehicle.
 */
class Navigator {
private:
    Mode *_curr_mode{nullptr};  // pointer to the current mode
    Mode *_modes[MODE_ARRAY_SIZE] {}; // pointer to an array of modes
    
public:
    /// Constructor
    Navigator(){}

    /// Disable copy constructor and assignment operator
    Navigator(const Navigator&) = delete;
    Navigator& operator=(const Navigator&) = delete;

    void run();
};