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

#include "mavlink_interface.h"

int main() {
    std::cout << "Initializing mavlink interface!" << std::endl;
    /// Initialize mavlink interface
    MavlinkInterface mav_interface;

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
    mav_interface.start();
    
    std::cout << "running! Press 'q' to stop." << std::endl;
    mav_interface.run();

    /// Wait for input thread to finish
    if (input_thread.joinable()) {
        input_thread.join();
    }
    return 0;
}