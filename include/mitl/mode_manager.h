/**
 * @file mode_manager.h
 * @author Abdulelah Mulla
 * @brief Header file for the system in charge of modes
 * @version 0.1
 * @date 2025-10-20
 */

#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <unordered_map>

#include "vehicle/vehicle.h"
#include "navigator/navigator.h"
#include "morb.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action_server/action_server.h>
#include <mavsdk/plugins/telemetry_server/telemetry_server.h>

/**
 * @brief The system that manages the mode
 * 
 * This class is in charge of running the current mode and 
 * updating the current mode based on requests made to it. 
 * 
 * All requests to change the mode go through this class, which
 * looks at the request and the current state of the vehicle and
 * decides the appropriate actions. 
 * 
 * Mode changes are governed by a state machine.
 * 
 */
class ModeManager {
private:
    /// The mode we are currently on
    mavsdk::ActionServer::FlightMode _curr_mode;

    /// Vehicle instance to utilize
    Vehicle& _vehicle;

    /// Action server instance to utilize
    mavsdk::ActionServer& _action;

    /// Message bus
    Morb *_morb;

    /// Navigator instance to utilize
    Navigator _navigator;

    /// Control thread running flag
    std::atomic<bool> _running;

    /// Dedicated control loop thread
    std::thread _control_thread;

    /// Thread safety lock
    std::mutex _mutex;

    /// The control rate we will be running at
    const int _CONTROL_RATE = 50;
    std::chrono::milliseconds _CONTROL_PERIOD = std::chrono::milliseconds(1000/_CONTROL_RATE);

    /**
     * @brief Verifies mode transitions
     * 
     * Uses the state machine outlined (TODO) to verify 
     * transitions.
     * 
     * MUST be called with mutex held
     * 
     * @param new_mode_type The mode type we want to change to
     * @return true if the transition is valid, false otherwise
     */
    bool isValidTransition(mavsdk::ActionServer::FlightMode new_mode_type);

    /**
     * @brief Main control loop
     * 
     * Execute at a fixed rate, running the current mode
     */
    void control_loop();
    
    /**
     * @brief Internal mode change (not thread-safe)
     * 
     * Validates the transition, exits current mode,
     * and enters the new mode
     * 
     * MUST be called with mutex held
     * 
     * @param new_mode_type The mode to switch to
     * @return true if transition successful
     */
    bool change_mode_internal(mavsdk::ActionServer::FlightMode new_mode_type);

    /**
     * @brief get the next mode type based on the current one
     * 
     * Based on state machine, determine what mode should
     * automatically transition to after current mode finishes
     * 
     * @param current Current mode type
     * @return Next mode type
     */
    mavsdk::ActionServer::FlightMode get_next_mode(mavsdk::ActionServer::FlightMode current);

public:

    explicit ModeManager(Vehicle& vehicle, mavsdk::ActionServer& action, Morb *morb);

    ~ModeManager();

    /**
     * @brief start the control loop 
     */
    void start();

    /**
     * @brief stop the control loop
     */
    void stop();

    /**
     * @brief Initialize all modes
     *
     * Creates instances of all mode types and adds them to the modes map.
     * Must be called before starting the control loop.
     */
    void initialize_modes();

    /**
     * @brief Changes to a new mode
     * 
     * Validates the transition, exits current mode,
     * and enters the new mode
     * 
     * @param mode The mode to switch to
     * @return true if transition successful
     */
    bool change_mode(mavsdk::ActionServer::FlightMode mode);

    /**
     * @brief Performs the takeoff operations
     * 
     * This function accepts the takoff request and verifies
     * the request based on the state-machine, and then gives
     * the job to the TakeOffMode.
     * 
     * @param height the height that we want to reach to
     */
    void activate_takeoff();

    /**
     * @brief Performs the landing operations
     * 
     * This function accepts the landing request and verifies
     * the request based on the state-machine, and then gives
     * the job to the LandMode. 
     */
    void activate_land();
};