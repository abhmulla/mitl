/**
 * @file vehicle.h
 * @author Abdulelah Mulla
 * @brief Header file for the Vehicle object
 * @version 0.1
 * @date 2025-10-21
 */

#pragma once

#include <memory>

#include "controller.h"
#include "position.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/server_component.h>
#include <mavsdk/plugins/action_server/action_server.h>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.h>
#include <mavsdk/plugins/telemetry_server/telemetry_server.h>

class Morb;

class Vehicle {
private:

    /// Server plugin to utilize
    std::shared_ptr<mavsdk::ServerComponent> _server;

    /// System plugin to utilize
    std::shared_ptr<mavsdk::System> _system;

    /// Mavlink direct instance to utilize
    std::shared_ptr<mavsdk::MavlinkDirect> _mavdirect;

    /// Telemetry plugin to utilize
    std::unique_ptr<mavsdk::TelemetryServer> _telem;

    /// Flag indicating if arming is in progress
    bool _arming_in_progress = false;

    /// Flag indicating if the vehicle is armed
    bool _armed = false;

    /// The current mode this vehicle is in
    mavsdk::ActionServer::FlightMode _curr_mode;

    /// Pointer to the controller object
    std::unique_ptr<Controller> _controller;

    /// Message bus
    Morb *_morb;

    /// Current position estimate
    Position _position;

    /// Vehicle State, HARDCODED FOR NOW
    mavsdk::TelemetryServer::Position _pos{42.7161389, -84.50325, 0.f, 0.f};
    mavsdk::TelemetryServer::PositionVelocityNed _pos_vel{{0,0,0},{0,0,0}};
    mavsdk::TelemetryServer::VelocityNed _vel{};
    mavsdk::TelemetryServer::Heading _hdg{60};
    mavsdk::TelemetryServer::RawGps _raw_gps{0,55.953251,-3.188267,0, NAN,NAN,0,NAN,0,0,0,0,0,0};
    mavsdk::TelemetryServer::GpsInfo _gps_info{11, mavsdk::TelemetryServer::FixType::Fix3D};
    mavsdk::TelemetryServer::Battery _battery{};

public:
    explicit Vehicle(std::shared_ptr<mavsdk::ServerComponent> server, std::shared_ptr<mavsdk::System> system, Morb* morb);

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
     * @brief Updates the current mode of the vehicle
     *
     * This is called by the ModeManager when the mode changes
     * so that telemetry can be published correctly
     *
     * @param mode The new mode
     */
    void set_mode(mavsdk::ActionServer::FlightMode mode);
};