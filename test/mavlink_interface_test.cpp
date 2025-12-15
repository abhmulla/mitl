/**
 * @file mavlink_interface_test.cpp
 * @author Abdulelah Mulla
 * @brief Unit tests for the mavlink interface
 * @version 0.1
 * @date 2025-12-15
 */

#include <atomic>
#include <chrono>
#include <thread>

#include "mavlink_interface.h"

#include <mavsdk/mavsdk.h>
#include <catch2/catch_test_macros.hpp>

using namespace mavsdk;

TEST_CASE("MavlinkInterface start initializes connections properly", "[MavlinkInterface]") {
    /// Initialize GCS for connection with the mavlink interface
    Mavsdk mavsdk_gcs{Mavsdk::Configuration{ComponentType::GroundStation}};
    auto result = mavsdk_gcs.add_any_connection("udpin://127.0.0.1:14550");
    REQUIRE(result == ConnectionResult::Success);

    /// Track when a system is discovered
    std::atomic<bool> system_discovered{false};
    std::shared_ptr<System> discovered_system = nullptr;

    /// Subscribe to new systems
    mavsdk_gcs.subscribe_on_new_system([&]() {
        auto systems = mavsdk_gcs.systems();
        if (!systems.empty()) {
            discovered_system = systems[0];
            system_discovered = true;
        }
    });

    /// Initialize mavlink interface with default arguments
    MavlinkInterface mav_interface;

    /// Start the mavlink interface
    bool start_result = mav_interface.start();
    REQUIRE(start_result);

    /// Wait for the GCS to discover the system (with timeout)
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!system_discovered && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    /// Verify that the system was discovered
    REQUIRE(system_discovered);
    REQUIRE(discovered_system != nullptr);
    REQUIRE(discovered_system->is_connected());

    /// Clean up
    mav_interface.stop();
}

/// TODO:
/// Test parameters
/// Test takeoff callback
/// Test land callback
/// Test mode change callback
/// Test vehicle loop (if telem is being published)
/// Test arm disarm
/// Test action setup
/// Test mission callback
/// Test mission server setup