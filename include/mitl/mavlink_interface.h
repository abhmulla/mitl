/**
 * @file mavlink_interface.h
 * @author Abdulelah Mulla
 * @brief Main header file for MITL
 * @version 0.1
 * @date 2025-10-19
 */

#pragma once

#include <atomic>
#include <future>
#include <iostream>
#include <thread>
#include <string>

#include <mavsdk/mavsdk.h>
#include <mavsdk/server_component.h>
#include <mavsdk/plugins/param_server/param_server.h>
#include <mavsdk/plugins/telemetry_server/telemetry_server.h>
#include <mavsdk/plugins/action_server/action_server.h>
#include <mavsdk/plugins/mission_raw_server/mission_raw_server.h>


class MavlinkInterface {
private:

    /// Connection URL to utilize
    std::string connection_url = "udpout://127.0.0.1:14550";

    /// MAVSDK configuration instance
    mavsdk::Mavsdk::Configuration config;

    /// MAVSDK instance to utilize
    mavsdk::Mavsdk mavsdk;

    /// Server plugin to utilize
    std::shared_ptr<mavsdk::ServerComponent> server;

    /// Param server plugin to utilize
    std::unique_ptr<mavsdk::ParamServer> param;

    /// Telemetry plugin to utilize
    std::unique_ptr<mavsdk::TelemetryServer> telem;

    /// Action plugin to utilize
    std::unique_ptr<mavsdk::ActionServer> action;

    /// Mission object
    std::unique_ptr<mavsdk::MissionRawServer>  mission;

    /// Mission sync
    std::promise<mavsdk::MissionRawServer::MissionPlan> mission_prom;
    std::future<mavsdk::MissionRawServer::MissionPlan> mission_future;
    mavsdk::MissionRawServer::IncomingMissionHandle mission_handle{};

    /// state
    std::atomic<bool> running{false};

    /// HARDCODED FOR NOW
    mavsdk::TelemetryServer::Position pos{55.953251, -3.188267, 0.f, 0.f};
    mavsdk::TelemetryServer::PositionVelocityNed pos_vel{{0,0,0},{0,0,0}};
    mavsdk::TelemetryServer::VelocityNed vel{};
    mavsdk::TelemetryServer::Heading hdg{60};
    mavsdk::TelemetryServer::RawGps raw_gps{0,55.953251,-3.188267,0, NAN,NAN,0,NAN,0,0,0,0,0,0};
    mavsdk::TelemetryServer::GpsInfo gps_info{11, mavsdk::TelemetryServer::FixType::Fix3D};
    mavsdk::TelemetryServer::Battery battery{};

    /**
     * @brief sets up the connection with the GCS
     * 
     * @return true if successful
     */
    bool setup_connection();

    /**
     * @brief Sets up the various plugins to the server object
     */
    void setup_plugins();

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
     * to takeoff. 
     * 
     * @param result Possible results returned for action requests
     * @param takeoff whether the takeoff command is still in progress
     */
    void on_takeoff(mavsdk::ActionServer::Result result, bool takeoff);

    /**
     * @brief Callback type for subscribe_land
     * 
     * This function is called by MAVSDK when the vehicle is commanded
     * to land. 
     * 
     * @param result Possible results returned for action requests
     * @param takeoff whether the takeoff command is still in progress
     */
    void on_land(mavsdk::ActionServer::Result result, bool land);

    /**
     * @brief Callback for receiving mission upload request from GCS
     * 
     * This function is called by MAVSDK when a mission is uploaded in a GCS.
     * 
     * @param result Tells you whether receiving the mission was successful
     * @param plan Contains the mission uploaded by the GCS
     */
    void on_incoming_mission(mavsdk::MissionRawServer::Result res,
                             mavsdk::MissionRawServer::MissionPlan plan);

public:

    MavlinkInterface(
        std::string url = "udpout://127.0.0.1:14550",
        mavsdk::ComponentType type = mavsdk::ComponentType::Autopilot);

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