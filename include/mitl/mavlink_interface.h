/**
 * @file mavlink_interface.h
 * @author Abdulelah Mulla
 * @brief Main header file for MITL
 * @version 0.2
 * @date 2025-10-19
 */

#pragma once

#include <atomic>
#include <future>
#include <iostream>
#include <thread>
#include <string>
#include <memory>

#include "mode_manager.h"
#include "vehicle.h"
#include "morb.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/param_server/param_server.h>
#include <mavsdk/plugins/action_server/action_server.h>
#include <mavsdk/plugins/mission_raw_server/mission_raw_server.h>

/**
 * @brief Mavlink interface for communicating with a GCS
 * 
 * This class represents the mavlink API that will be used maintain
 * communication with a GCS. 
 */
class MavlinkInterface {
private:

    /// Connection URL to utilize
    std::string _connection_url = "udpout://127.0.0.1:14550";

    /// MAVSDK configuration instance
    mavsdk::Mavsdk::Configuration _config;

    /// MAVSDK instance to utilize
    mavsdk::Mavsdk _mavsdk;

    /// Server plugin to utilize
    std::shared_ptr<mavsdk::ServerComponent> _server;

    /// System instance to utilize
    std::shared_ptr<mavsdk::System> _system;

    /// Param server plugin to utilize
    std::unique_ptr<mavsdk::ParamServer> _param;

    /// Action plugin to utilize
    std::unique_ptr<mavsdk::ActionServer> _action;

    /// The vehicle object
    std::unique_ptr<Vehicle> _vehicle;

    /// Mode manager
    std::unique_ptr<ModeManager> _manager;

    /// This will allow us to receive missions from a GCS
    std::unique_ptr<mavsdk::MissionRawServer>  _mission;

    /// Mission sync
    std::promise<mavsdk::MissionRawServer::MissionPlan> _mission_prom;
    std::future<mavsdk::MissionRawServer::MissionPlan> _mission_future;
    mavsdk::MissionRawServer::IncomingMissionHandle _mission_handle{};

    /// state
    std::atomic<bool> _running{false};

    /// Dedicated thread for 'running' the vehicle
    std::thread _vehicle_thread;

    /// Flag indicating that the vehicle is armed
    std::atomic<bool> _armed{false};

    /// Message bus
    std::unique_ptr<Morb> _morb;
    
    /**
     * @brief sets up the connection with the GCS
     * 
     * @return true if successful
     */
    bool setup_connection();

    /**
     * @brief Sets up the parameters of our vehicle
     */
    void setup_params();

    /**
     * @brief Sets up the actions of our vehicle
     * 
     * Subscribes to the takeoff and landing callbacks.
     */
    void setup_actions();

    /**
     * @brief Sets up the MissionRawServer
     * 
     * Subscribes to incoming mission callback.
     */
    void setup_mission_server();

    /**
     * @brief Callback type for subscribe_takeoff
     * 
     * This function is called by MAVSDK when the vehicle is commanded
     * to takeoff. If this function is called, we know that there is a 
     * takeoff request.
     * 
     * @param result Possible results returned for action requests
     * @param takeoff whether the takeoff command is still in progress
     */
    void on_takeoff(mavsdk::ActionServer::Result result, bool takeoff);

    /**
     * @brief Callback type for subscribe_land
     * 
     * This function is called by MAVSDK when the vehicle is commanded
     * to land. If this function is called, we know that there is a 
     * landing request.
     * 
     * @param result Possible results returned for action requests
     * @param takeoff whether the takeoff command is still in progress
     */
    void on_land(mavsdk::ActionServer::Result result, bool land);

    /**
     * @brief Callback of arming and disarming.
     * 
     * This function is called by MAVSDK when the vehicle is commanded
     * to arm or disarm. If this function is called, we know that there
     * is a request to arm or disarm the vehicle. 
     */
    void on_arm_disarm(mavsdk::ActionServer::Result result, mavsdk::ActionServer::ArmDisarm arm_disarm);

    /**
     * @brief Callback for receiving mission upload request from GCS
     * 
     * This function is called by MAVSDK when a mission is uploaded in a GCS.
     * 
     * @param result Tells you whether receiving the mission was successful
     * @param plan Contains the mission uploaded by the GCS
     */
    void on_incoming_mission(mavsdk::MissionRawServer::Result res, mavsdk::MissionRawServer::MissionPlan plan);
    
    /**
     * @brief Callback for receiving mode change requests from the GCS
     */
    void on_mode_change(mavsdk::ActionServer::Result res, mavsdk::ActionServer::FlightMode mode);

    /**
     * @brief Performs necessary vehicle functions for the vehicle thread
     * 
     * Simply publishes telem and sends heartbeats for now
     */
    void vehicle_loop();

public:

    MavlinkInterface(
        std::string url = "udpout://127.0.0.1:14550", mavsdk::ComponentType type = mavsdk::ComponentType::Autopilot);

    MavlinkInterface(const MavlinkInterface&) = delete;

    MavlinkInterface& operator=(const MavlinkInterface&) = delete;

    /**
     * @brief Performs all required start operations
     * 
     * This function prepares this instance for communicating
     * with a system via MAVLINK.
     * We perform the following:
     * 
     * - Create required components and structures
     * - Connect to any added systems and determine if they are eligible
     * - Add callback functions to react to incoming telemetry data
     * 
     * All these steps are REQUIRED for proper functionality,
     * and this function MUST be called before any operations are preformed. 
     * 
     * @return bool true if successful, false if not
     */ 
    bool start();

    /**
     * @brief Publishes the required telemetry
     * 
     * This function publishes telemetry data to the GCS
     * with the telemetry object via MAVLINK.
     */
    void run();

    /**
     * @brief Preforms all required stop operations
     * 
     * This function destroys the MAVSDK object.
     * This will ensure all resources are freed and all background threads are killed.
     * This function will be called automatically when this object is destroyed,
     * but it can be called automatically if necessary.
     * 
     * Once a telemetry object is stopped,
     * then it CAN'T be restarted or used again!
     */
    void stop();
};