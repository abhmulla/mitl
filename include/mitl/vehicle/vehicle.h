/**
 * @file vehicle.h
 * @author Abdulelah Mulla
 * @brief Header file for the Vehicle object
 * @version 0.1
 * @date 2025-10-21
 */

#pragma once

#include <memory>

#include "mode/mode.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/server_component.h>
#include <mavsdk/plugins/action_server/action_server.h>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.h>
#include <mavsdk/plugins/telemetry_server/telemetry_server.h>


/**
 * @brief The states our vehicle can be in
 * 
 * This enum defines possible states our vehicle can be in, 
 * which are used in managing the vehicles mode. 
 */
enum State {
    Ground,  ///< The vehicle is on the ground
    Takeoff, ///< The vehicle is taking off
    Hold, ///< The vehicle is unmoving in the air
    Heading, ///< The vehicle is heading to a waypoint
    Land ///< The vehicle is landing
};

class Vehicle {
private:

    /// Server plugin to utilize
    std::shared_ptr<mavsdk::ServerComponent> server;

    /// System plugin to utilize
    std::shared_ptr<mavsdk::System> system;

    /// Mavlink direct instance to utilize
    std::shared_ptr<mavsdk::MavlinkDirect> mavdirect;

    /// Telemetry plugin to utilize
    std::unique_ptr<mavsdk::TelemetryServer> telem;

    /// Flag indicating if arming is in progress
    bool arming_in_progress = false;

    /// Flag indicating if the vehicle is armed
    bool armed = false;

    /// The current mode this vehicle is in
    ModeType curr_mode;

    /// HARDCODED FOR NOW
    mavsdk::TelemetryServer::Position pos{42.7161389, -84.50325, 0.f, 0.f};
    mavsdk::TelemetryServer::PositionVelocityNed pos_vel{{0,0,0},{0,0,0}};
    mavsdk::TelemetryServer::VelocityNed vel{};
    mavsdk::TelemetryServer::Heading hdg{60};
    mavsdk::TelemetryServer::RawGps raw_gps{0,55.953251,-3.188267,0, NAN,NAN,0,NAN,0,0,0,0,0,0};
    mavsdk::TelemetryServer::GpsInfo gps_info{11, mavsdk::TelemetryServer::FixType::Fix3D};
    mavsdk::TelemetryServer::Battery battery{};

public:
    explicit Vehicle(std::shared_ptr<mavsdk::ServerComponent> server, std::shared_ptr<mavsdk::System> system):
    server(server), system(system) {
        telem = std::make_unique<mavsdk::TelemetryServer>(server);
        mavdirect = std::make_unique<mavsdk::MavlinkDirect>(system);
    }

    /**
     * @brief arm the vehicle
     */
    void arm();

    /**
     * @brief disarm the vehicle
     */
    void disarm();

    /**
     * @brief Let's us know if the vehicle is armed
     * 
     * This returns true if the vehicle is done arming
     * 
     * @return true if armed, false otherwise
     */
    bool is_armed();

    /**
     * @brief Let's us know if the vehicle is in the process of arming
     * 
     * @return true if the vehicle is arming, false otherwise
     */
    bool is_arming();

    /**
     * @brief publishes telemetry using mavlink
     */
    void publish_telem();

    /**
     * @brief performs takeoff operations
     */
    void takeoff();

    /**
     * @brief performs necessary operations to hold
     */
    void enter_hold();

    /**
     * @brief sends a mavlink heartbeat message when calles
     */
    // void send_heartbeat();
};