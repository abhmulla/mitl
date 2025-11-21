/**
 * @file mode_manager.cpp
 * @author Abdulelah Mulla
 */

#include "mode_manager.h"
#include "vehicle/controller.h"
#include <iostream>


/// Destructor
ModeManager::~ModeManager() {
    stop();
}

void ModeManager::initialize_modes() {
    std::cout << "[ModeManager] Initializing modes..." << std::endl;
    /// Start in Ground mode
    curr_mode = mavsdk::ActionServer::FlightMode::Ready;
/// TODO:

    //curr_mode->enter();
    //navigator->enter(curr_mode)

    /// Update vehicle's mode for telemetry
    vehicle.set_mode(mavsdk::ActionServer::FlightMode::Ready);

    std::cout << "[ModeManager] All modes initialized, starting in Ground mode" << std::endl;
}

void ModeManager::start() {
    if (running.load()) {
        return;
    }
    running.store(true);
    control_thread = std::thread(&ModeManager::control_loop, this);
}

void ModeManager::stop() {
    if (!running.load()) {
        return;
    }
    running.store(false);
    if (control_thread.joinable()) {
        control_thread.join();
    }
}

/// BIG TODO: Adust it for the navigator
void ModeManager::control_loop() {
    while (running.load()) {
        auto loop_start =std::chrono::high_resolution_clock::now();
        {
            /// Lock mutex for the duration of this update cycle
            std::lock_guard<std::mutex> lock(mutex);
            // if (curr_mode != nullptr) {
            //     /// Run current mode logic
            //     curr_mode->update();
            //     /// Check if mode is complete and auto-transition
            //     if (curr_mode->is_complete()) {
            //         ModeType next = get_next_mode(curr_mode->get_type());
            //         change_mode_internal(next);
            //     }
            // }
        }
        /// We release the mutex
        /// Sleep for remainder of control period to maintain fixed rate
        auto loop_end = std::chrono::high_resolution_clock::now();
        auto elapsed = loop_end - loop_start;
        if (elapsed < CONTROL_PERIOD) {
            std::this_thread::sleep_for(CONTROL_PERIOD - elapsed);
        } else {
            /// Log if we're running slower than desired rate
            std::cerr << "Warning: Control loop took longer than anticipated" << std::endl;
        }
    }
}

bool ModeManager::isValidTransition(mavsdk::ActionServer::FlightMode new_mode_type) {
    switch (curr_mode) {
        case mavsdk::ActionServer::FlightMode::Ready:
            return new_mode_type == mavsdk::ActionServer::FlightMode::Takeoff;

        case mavsdk::ActionServer::FlightMode::Takeoff:
            return new_mode_type == mavsdk::ActionServer::FlightMode::Hold;

        case mavsdk::ActionServer::FlightMode::Hold:
            return new_mode_type == mavsdk::ActionServer::FlightMode::Land ||
                   new_mode_type == mavsdk::ActionServer::FlightMode::Mission;

        case mavsdk::ActionServer::FlightMode::Mission:
            return new_mode_type == mavsdk::ActionServer::FlightMode::Hold;

        case mavsdk::ActionServer::FlightMode::Land:
            return new_mode_type == mavsdk::ActionServer::FlightMode::Ready;

        default:
            return false;
    }
}

mavsdk::ActionServer::FlightMode ModeManager::get_next_mode(mavsdk::ActionServer::FlightMode current) {
    /// Automatic transitions when a mode completes
    switch (current) {
        case mavsdk::ActionServer::FlightMode::Takeoff:
            return mavsdk::ActionServer::FlightMode::Hold;

        case mavsdk::ActionServer::FlightMode::Mission:
            return mavsdk::ActionServer::FlightMode::Hold;

        case mavsdk::ActionServer::FlightMode::Land:
            return mavsdk::ActionServer::FlightMode::Ready;

        default:
            return current;
    }
}

bool ModeManager::change_mode_internal(mavsdk::ActionServer::FlightMode new_mode_type) {
    /// Check if mode exists
    if (new_mode_type == mavsdk::ActionServer::FlightMode::Unknown) {
        return false;
    }
    /// Validate transition
    if (!isValidTransition(new_mode_type)) {
        return false;
    }
    /// Perform transition
    action.set_flight_mode(new_mode_type);
// TODO:
    // curr_mode = modes[new_mode_type].get();
    // curr_mode->enter();
    // tell the mode change either with a message or through navigator
    /// Update vehicle's mode for telemetry
    vehicle.set_mode(new_mode_type);
    return true;
}

bool ModeManager::change_mode(mavsdk::ActionServer::FlightMode mode) {
    std::lock_guard<std::mutex> lock(mutex);
    return change_mode_internal(mode);
}

void ModeManager::activate_takeoff() {
    std::lock_guard<std::mutex> lock(mutex);
    /// Check if we need to arm first
    if (!vehicle.is_armed()) {
        if (!vehicle.is_arming()) {
            vehicle.arm();
        }
        /// We wait for the next call if arming is not done
        return;
    }
    change_mode_internal(mavsdk::ActionServer::FlightMode::Takeoff);
}

void ModeManager::activate_land() {
    std::lock_guard<std::mutex> lock(mutex);
    change_mode(mavsdk::ActionServer::FlightMode::Land);
}