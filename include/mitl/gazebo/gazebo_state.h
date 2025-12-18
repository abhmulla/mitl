/**
 * @file gazebo_state.h
 * @author Abdulelah Mulla
 * @brief Code for receiving state updates
 * @version 0.1
 * @date 2025-12-03
 */

#pragma once

#include "morb.h"
#include <gz/msgs.hh>
#include <gz/transport.hh>

/**
 * @brief This class is responsible for subscribing 
 * to the necessary gazebo topics for sensing and
 * state estimation.
 * 
 * Currently, we hardcode to subscribe to the gz_x500 vehicle and 
 * assume the world is the default world.
 * 
 * TODO: given a vehicle, check if its valid (from a list of options) and
 * use that. Given a world, check if its valid, and use that.
 */
class GazeboState {
private:
    /// Message bus
    Morb *_morb;
public:
    /**
     * Constructor
     */
    GazeboState(Morb* morb);

    /**
     * @brief Subscribes to sensor readings and publishes
     * them to mitl via morb.
     * 
     * The sensor readings include: Accelerometer, Gyroscope,
     * Magnetometer, Magnetometer, Barometer
     */
    void activate_subscriptions();

    /**
     * @brief Publishes actuator commands to Gazebo.
     * 
     * Commands come from the controller.
     */
    void publish_actuator();
};