/**
 * @file gazebo.cpp
 * @author Abdulelah Mulla
 * @brief demonstration of gazebo_state subscriptions
 * */

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>

#include "gazebo/gazebo_state.h"
#include "scheduler.h"
#include "morb.h"

int main() {
    std::atomic<bool> stop_requested(false);

    /// Thread to listen for user input
    std::thread input_thread([&]() {
        char c;
        while (std::cin >> c) {
            if (c == 'q') {
                std::cout << "Stop requested by user.\n";
                stop_requested = true;
                break;
            }
        }
    });

    std::cout << "starting..." << std::endl;

    std::string world = "default", vehicle = "x500_0";
    Scheduler::initialize();
    Morb morb;
    GazeboState gazebo_state(&morb, world, vehicle);
    gazebo_state.activate_subscriptions();

    std::cout << "running! Press 'q' to stop." << std::endl;

    while (!stop_requested) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    /// Wait for input thread to finish
    if (input_thread.joinable()) {
        input_thread.join();
    }
    
    return 0;
}