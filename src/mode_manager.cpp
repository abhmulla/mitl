/**
 * @file mode_manager.cpp
 * @author Abdulelah Mulla
 */

#include "mode_manager.h"
#include <iostream>


/// Destructor
ModeManager::~ModeManager() {
    stop();
}

void ModeManager::initialize_modes() {
    std::cout << "[ModeManager] Initializing modes..." << std::endl;

    /// Create all mode instances
    modes[ModeType::Ground] = std::make_unique<GroundMode>(vehicle);
    modes[ModeType::Takeoff] = std::make_unique<TakeOffMode>(vehicle);
    modes[ModeType::Hold] = std::make_unique<HoldMode>(vehicle);
    modes[ModeType::Heading] = std::make_unique<HeadingMode>(vehicle);
    modes[ModeType::Land] = std::make_unique<LandMode>(vehicle);

    /// Start in Ground mode
    curr_mode = modes[ModeType::Ground].get();
    curr_mode->enter();

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

void ModeManager::control_loop() {
    while (running.load()) {
        auto loop_start =std::chrono::high_resolution_clock::now();
        {
            /// Lock mutex for the duration of this update cycle
            std::lock_guard<std::mutex> lock(mutex);
            if (curr_mode != nullptr) {
                /// Run current mode logic
                curr_mode->update();
                /// Check if mode is complete and auto-transition
                if (curr_mode->is_complete()) {
                    ModeType next = get_next_mode(curr_mode->get_type());
                    change_mode_internal(next);
                }
            }
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

bool ModeManager::isValidTransition(ModeType new_mode_type) {
    if (curr_mode == nullptr) {
        return new_mode_type == ModeType::Ground;
    }
    ModeType curr_mode_type = curr_mode->get_type();
    switch (curr_mode_type) {
        case ModeType::Ground:
            return new_mode_type == ModeType::Takeoff;

        case ModeType::Takeoff:
            return new_mode_type == ModeType::Hold;

        case ModeType::Hold:
            return new_mode_type == ModeType::Land ||
                   new_mode_type == ModeType::Heading;

        case ModeType::Heading:
            return new_mode_type == ModeType::Hold;

        case ModeType::Land:
            return new_mode_type == ModeType::Ground;

        default:
            return false;
    }
}

ModeType ModeManager::get_next_mode(ModeType current) {
    /// Automatic transitions when a mode completes
    switch (current) {
        case ModeType::Takeoff:
            return ModeType::Hold;

        case ModeType::Heading:
            return ModeType::Hold;

        case ModeType::Land:
            return ModeType::Ground;

        default:
            return current;
    }
}

bool ModeManager::change_mode_internal(ModeType new_mode_type) {
    /// Check if mode exists
    if (modes.find(new_mode_type) == modes.end()) {
        return false;
    }
    /// Validate transition
    if (!isValidTransition(new_mode_type)) {
        return false;
    }
    /// Perform transition
    if (curr_mode != nullptr) {
        curr_mode->exit();
    }
    curr_mode = modes[new_mode_type].get();
    curr_mode->enter();
    return true;
}

bool ModeManager::change_mode(ModeType new_mode_type) {
    std::lock_guard<std::mutex> lock(mutex);
    return change_mode_internal(new_mode_type);
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
    change_mode_internal(ModeType::Takeoff);
}

void ModeManager::activate_land() {
    std::lock_guard<std::mutex> lock(mutex);
    /// If currently in Heading mode, must go through Hold first
    if (curr_mode != nullptr && curr_mode->get_type() == ModeType::Heading) {
        change_mode_internal(ModeType::Hold);
        return;
    }
    change_mode_internal(ModeType::Land);
}