/**
 * @file main.cpp
 * @author Abdulelah Mulla
 * @brief Main implementation of mitl
 * @version 0.1
 * @date 2025-10-19
 */

#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <fstream>
#include <string.h> 

#include "morb.h"
#include "mavlink_interface.h"
#include "gazebo/gazebo_state.h"
#include "scheduler.h"
#include "log.h"

/**
 * The main implementation. This serves as the
 * startup script that launches all modules. When executing
 * the binary, the program takes two optional arguments.
 * --world=<name>: Name of the Gazebo world (default: "default")
 * --vehicle=<name>: Name of the vehicle model (default: "x500_0")
 */
int main(int argc, char *argv[]) {
    /// Parse arguments
    std::string world = "default";
    std::string vehicle = "x500_0";

    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);

        if (arg.find("--world=") == 0) {
            world = arg.substr(8);
            if (world.empty()) {
                std::cout << "Error: --world= requires a value" << std::endl;
                return 1;
            }
        } else if (arg.find("--vehicle=") == 0) {
            vehicle = arg.substr(10);
            if (vehicle.empty()) {
                std::cout << "Error: --vehicle= requires a value" << std::endl;
                return 1;
            }
        } else {
            std::cout << "Unknown argument: " << arg << std::endl;
            std::cout << "Usage: " << argv[0] << " [--world=<name>] [--vehicle=<name>]" << std::endl;
            return 1;
        }
    }
    /// Initialize morb
    Morb morb;
    /// Start scheduler
    Scheduler::initialize();
    /// Initialize gazebo_state
    GazeboState gazebo_state(&morb, world, vehicle);
    gazebo_state.activate_subscriptions();
    /// Initialize mavlink interface
    MavlinkInterface mav_interface(&morb);

    std::atomic<bool> stop_requested{false};

    /// Thread to listen for user input
    std::thread input_thread([&]() {
        char c;
        while (std::cin >> c) {
            if (c == 'q') {
                std::cout << "Stop requested by user.\n";
                stop_requested = true;
                mav_interface.stop();
                break;
            }
        }
    });

    std::cout << "starting..." << std::endl;
    if (!mav_interface.start()) {
        std::cerr << "Failed to start mavlink interface!" << std::endl;
        stop_requested = true;
        if (input_thread.joinable()) {
            input_thread.detach();
        }
        return 1;
    }

    std::cout << "running! Press 'q' to stop." << std::endl;
    mav_interface.run();

    /// Wait for input thread to finish
    if (input_thread.joinable()) {
        input_thread.join();
    }
    return 0;
}