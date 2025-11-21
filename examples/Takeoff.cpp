/**
 * @file Takeoff.cpp
 * @author Abdulelah Mulla
 * @date 25-11-07
 * @brief Connects to the autopilot and executes a takeoff command.
 */

#include <iostream>
#include <future>
#include <memory>
#include <chrono>
#include <thread>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <plugins/telemetry/telemetry.h>

int main() {
    
    std::string connection_url = "udp://:14550";
    std::cout << "Listending on " << connection_url << std::endl;

    /// Initialize mavsdk object
    mavsdk::Mavsdk mavsdk{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::GroundStation}};

    mavsdk::ConnectionResult conn_result = mavsdk.add_any_connection(connection_url);

    if (conn_result != mavsdk::ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_url << '\n';
        return 0;
    }

    std::promise<std::shared_ptr<mavsdk::System>> prom;
    std::future<std::shared_ptr<mavsdk::System>> fut = prom.get_future();

    const mavsdk::Mavsdk::NewSystemHandle handle = mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
        auto systems = mavsdk.systems();
        std::cout << "Number of systems detected: " << systems.size() << '\n';

        if (!systems.empty()) {
            auto system = systems.at(0);
            if (system->has_autopilot()) {
                std::cout << "Drone discovered!" << '\n';

                prom.set_value(system);

            } else {
                std::cout << "Detected system does not have an autopilot."
                          << '\n';
                prom.set_value(nullptr);
            }
        } else {
            std::cout << "No systems found." << '\n';
            prom.set_value(nullptr);
        } 
    });

    auto system = fut.get();
    if (!system) {
        std::cerr << "Failed to connect to the drone." << '\n';
        return 0;
    }

    // Remove system callback:

    mavsdk.unsubscribe_on_new_system(handle);

    if (!system->is_connected()) {
        std::cerr << "System is not connected!" << '\n';
        return 0;
    }

    auto action = mavsdk::Action{system};

     std::unique_ptr<mavsdk::Telemetry> telemetry = std::make_unique<mavsdk::Telemetry>(system);

    // Wait until health is OK and vehicle is ready to arm
    while (telemetry->health_all_ok() != true) {
        std::cout << "Vehicle not ready to arm ..." << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Exit if calibration is required
    // mavsdk::Telemetry::Health check_health = telemetry->health();
    // bool calibration_required = false;
    // if (!check_health.gyrometer_calibration_ok) {
    //     std::cout << "Gyro requires calibration." << '\n';
    //     calibration_required=true;
    // }
    // if (!check_health.accelerometer_calibration_ok) {
    //     std::cout << "Accelerometer requires calibration." << '\n';
    //     calibration_required=true;
    // }
    // if (!check_health.magnetometer_calibration_ok) {
    //     std::cout << "Magnetometer (compass) requires calibration." << '\n';
    //     calibration_required=true;
    // }
    // if (!check_health.level_calibration_ok) {
    //     std::cout << "Level calibration required." << '\n';
    //     calibration_required=true;
    // }
    // if (calibration_required) {
    //     return 1;
    // }


    // // Check if ready to arm (reporting status)
    // while (telemetry->health_all_ok() != true) {
    //     std::cout << "Vehicle not ready to arm. Waiting on:" << '\n';
    //     mavsdk::Telemetry::Health current_health = telemetry->health();
    //     if (!current_health.global_position_ok) {
    //         std::cout << "  - GPS fix." << '\n';
    //     }
    //     if (!current_health.local_position_ok) {
    //         std::cout << "  - Local position estimate." << '\n';
    //     }
    //     if (!current_health.home_position_ok) {
    //         std::cout << "  - Home position to be set." << '\n';
    //     }
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }

    // Arm vehicle
    std::cout << "Arming..." << '\n';
    const mavsdk::Action::Result arm_result = action.arm();

    if (arm_result != mavsdk::Action::Result::Success) {
        std::cout << "Arming failed:" << arm_result <<  '\n';
        return 1; //Exit if arming fails
    }
    std::cout << "Taking off..." << '\n';
    const mavsdk::Action::Result takeoff_result = action.takeoff();
    if (takeoff_result != mavsdk::Action::Result::Success) {
        std::cout << "Takeoff failed:" << takeoff_result << '\n';
        return 1;
    }
}