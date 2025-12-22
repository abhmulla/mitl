/**
 * @file gazebo_state.h
 * @author Abdulelah Mulla
 * @brief Code for receiving state updates
 * @version 0.1
 * @date 2025-12-03
 */

#pragma once

#include <string>
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
 */
class GazeboState {
private:
    /// Message bus
    Morb *_morb;
    /// World name
    const std::string _world;
    /// Vehicle name
    const std::string _vehicle;
    
    gz::transport::Node _node;

    /// Callbacks
    void clock_callback(const gz::msgs::Clock &msg);
    void airspeed_callback(const gz::msgs::AirSpeed &msg);
	void air_pressure_callback(const gz::msgs::FluidPressure &msg);
	void imu_callback(const gz::msgs::IMU &msg);
	void pose_info_callback(const gz::msgs::Pose_V &msg);
	void odometry_callback(const gz::msgs::OdometryWithCovariance &msg);
	void nav_sat_callback(const gz::msgs::NavSat &msg);
	void lidar_callback(const gz::msgs::LaserScan &msg);
	void laser_scan_callback(const gz::msgs::LaserScan &msg);
	void mag_callback(const gz::msgs::Magnetometer &msg);
public:
    /**
     * Constructor
     */
    GazeboState(Morb* morb, std::string world, std::string vehicle);

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