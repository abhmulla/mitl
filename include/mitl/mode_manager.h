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
#include "mode/mode.h"
#include "mode/groundmode.h"
#include "mode/takeoffmode.h"
#include "mode/holdmode.h"
#include "mode/headingmode.h"
#include "mode/landmode.h"

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

    /// All the available modes
    std::unordered_map<ModeType, std::unique_ptr<Mode>> modes;

    /// The mode we are currently on
    Mode* curr_mode;

    /// Vehicle instance to utilize
    Vehicle& vehicle;

    /// Action server instance to utilize
    mavsdk::ActionServer& action;

    /// Control thread running flag
    std::atomic<bool> running;

    /// Dedicated control loop thread
    std::thread control_thread;

    /// Thread safety lock
    std::mutex mutex;

    /// The control rate we will be running at
    const int CONTROL_RATE = 50;
    std::chrono::milliseconds CONTROL_PERIOD = std::chrono::milliseconds(1000/CONTROL_RATE);

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
    bool isValidTransition(ModeType new_mode_type);

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
    bool change_mode_internal(ModeType new_mode_type);

    /**
     * @brief get the next mode type based on the current one
     * 
     * Based on state machine, determine what mode should
     * automatically transition to after current mode finishes
     * 
     * @param current Current mode type
     * @return Next mode type
     */
    ModeType get_next_mode(ModeType current);

    /**
     * @brief map FlightMode enum to our ModeType
     * 
     * Used to simplify internal mode switching operations
     * 
     * @return The corresponding modetype
     */
    ModeType to_modetype(mavsdk::ActionServer::FlightMode mode);

    /**
     * @brief Maps our custom modes to the FlightMode enum for mavlink communication
     * 
     * MAVSDK internally uses px4's custom modes, so we map our mode's to 
     * action server's Flight mode enum, which is then handled internally by MAVSDK 
     * with the px4's custom modes.
     * 
     * @return the matching px4 custom mode
     */
    mavsdk::ActionServer::FlightMode to_action_server_mode(ModeType mode) const;

public:

    explicit ModeManager(Vehicle& vehicle, mavsdk::ActionServer& action) :
     vehicle(vehicle), action(action) {}

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