/**
 * @file gazebo_state.cpp
 * @author Abdulelah Mulla
 */

#include <string>
#include <iostream>

#include "gazebo/gazebo_state.h" 
#include "scheduler.h"
#include "log.h"

/// Constructor
GazeboState::GazeboState(Morb* morb, std::string world, std::string vehicle) :
    _morb(morb),
    _world(world),
    _vehicle(vehicle) 
    {

}

void GazeboState::activate_subscriptions() {
    /// Clock
    std::string clock_string = "/world/" + _world + "/clock";
    if(!_node.Subscribe(clock_string, &GazeboState::clock_callback, this)) {
        std::cerr << "Error subscribing to clock topic" << std::endl;
    } else {
        mitl_log << "[GazeboState] Subscribed to Clock topic" << std::endl;
    }
    /// Pose Info
    std::string pose_string = "/world/" + _world + "/pose/info";
    if(!_node.Subscribe(pose_string, &GazeboState::pose_info_callback, this)) {
        std::cerr << "Error subscribing to pose info topic" << std::endl;
    } else {
        mitl_log << "[GazeboState] Subscribed to pose info topic" << std::endl;
    }
    /// IMU
    std::string imu_string = "/world/" + _world + "/model/" + _vehicle + "/link/base_link/sensor/imu_sensor/imu";
    if(!_node.Subscribe(imu_string, &GazeboState::imu_callback, this)) {
        std::cerr << "Error subscribing to imu topic" << std::endl;
    } else {
        mitl_log << "[GazeboState] Subscribed to imu topic" << std::endl;
    }
    /// Mag
    std::string mag_string = "/world/" + _world + "/model/" + _vehicle +
				"/link/base_link/sensor/magnetometer_sensor/magnetometer";
    if(!_node.Subscribe(mag_string, &GazeboState::mag_callback, this)) {
        std::cerr << "Error subscribing to magnetometer topic" << std::endl;
    } else {
        mitl_log << "[GazeboState] Subscribed to magnetometer topic" << std::endl;
    }
    /// Odo
    std::string odometry_string = "/model/" + _vehicle + "/odometry_with_covariance";
    if(!_node.Subscribe(odometry_string, &GazeboState::odometry_callback, this)) {
        std::cerr << "Error subscribing to odometry topic" << std::endl;
    } else {
        mitl_log << "[GazeboState] Subscribed to odometry topic" << std::endl;
    }
    /// LaserScan
    std::string laser_scan_string = "/world/" + _world + "/model/" + _vehicle + "/link/link/sensor/lidar_2d_v2/scan";
    if(!_node.Subscribe(laser_scan_string, &GazeboState::laser_scan_callback, this)) {
        std::cerr << "Error subscribing to laser scan topic" << std::endl;
    } else {
        mitl_log << "[GazeboState] Subscribed to laser scan topic" << std::endl;
    }
    /// Lidar Sensor
    std::string lidar_sensor_string = "/world/" + _world + "/model/" + _vehicle +
				   "/link/lidar_sensor_link/sensor/lidar/scan";
    if(!_node.Subscribe(lidar_sensor_string, &GazeboState::lidar_callback, this)) {
        std::cerr << "Error subscribing to lidar topic" << std::endl;
    } else {
        mitl_log << "[GazeboState] Subscribed to lidar topic" << std::endl;
    }
    /// Airspeed
    std::string airspeed_string = "/world/" + _world + "/model/" + _vehicle +
				     "/link/airspeed_link/sensor/air_speed/air_speed";
    if(!_node.Subscribe(airspeed_string, &GazeboState::airspeed_callback, this)) {
        std::cerr << "Error subscribing to airspeed topic" << std::endl;
    } else {
        mitl_log << "[GazeboState] Subscribed to airspeed topic" << std::endl;
    }
    /// Airpressure
    std::string air_pressure_string = "/world/" + _world + "/model/" + _vehicle +
					 "/link/base_link/sensor/air_pressure_sensor/air_pressure";
    if(!_node.Subscribe(air_pressure_string, &GazeboState::air_pressure_callback, this)) {
        std::cerr << "Error subscribing to air pressure topic" << std::endl;
    } else {
        mitl_log << "[GazeboState] Subscribed to air pressure topic" << std::endl;
    }
    /// Navsat
    std::string nav_sat_string = "/world/" + _world + "/model/" + _vehicle +
				    "/link/base_link/sensor/navsat_sensor/navsat";
    if(!_node.Subscribe(nav_sat_string, &GazeboState::nav_sat_callback, this)) {
        std::cerr << "Error subscribing to navsat topic" << std::endl;
    } else {
        mitl_log << "[GazeboState] Subscribed to navsat topic" << std::endl;
    }
}

void GazeboState::publish_actuator() {

}

/// Callbacks

void GazeboState::clock_callback(const gz::msgs::Clock &msg) {
    /// Convert timespec to absolute time
    uint64_t time_mcs = (uint64_t)msg.sim().sec() * 1000000;
    time_mcs += (uint64_t)(msg.sim().nsec() / 1000);
    /// Set time
    Scheduler::initialize().set_time(time_mcs);
}

void GazeboState::airspeed_callback(const gz::msgs::AirSpeed &msg) {

}

void GazeboState::air_pressure_callback(const gz::msgs::FluidPressure &msg) {

}

void GazeboState::imu_callback(const gz::msgs::IMU &msg) {
    
}

void GazeboState::pose_info_callback(const gz::msgs::Pose_V &msg) {

}

void GazeboState::odometry_callback(const gz::msgs::OdometryWithCovariance &msg) {
    
}

void GazeboState::nav_sat_callback(const gz::msgs::NavSat &msg) {
    
}

void GazeboState::lidar_callback(const gz::msgs::LaserScan &msg) {
    
}

void GazeboState::laser_scan_callback(const gz::msgs::LaserScan &msg) {
    
}

void GazeboState::mag_callback(const gz::msgs::Magnetometer &msg) {
    
}