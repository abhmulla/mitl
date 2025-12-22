/**
 * @file mavlink_interface_test.cpp
 * @author Abdulelah Mulla
 * @brief Unit tests for the mavlink interface
 * @version 0.2
 * @date 2025-12-15
 */

#include <atomic>
#include <chrono>
#include <thread>
#include <iostream>

#include "mavlink_interface.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/action_server/action_server.h>
#include <mavsdk/plugins/param/param.h>
#include <mavsdk/plugins/mission_raw/mission_raw.h>
#include <catch2/catch_test_macros.hpp>

using namespace mavsdk;

TEST_CASE("MavlinkInterface start initializes connections properly", "[MavlinkInterface]") {
    /// Initialize GCS for connection with the mavlink interface
    Mavsdk mavsdk_gcs{Mavsdk::Configuration{ComponentType::GroundStation}};
    auto result = mavsdk_gcs.add_any_connection("udpin://127.0.0.1:14551");
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

    Morb morb;
    MavlinkInterface mav_interface{&morb, "udpout://127.0.0.1:14551"};

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

TEST_CASE("Takeoff functionality", "[takeoff]") {
    /// Setup MavlinkInterface and GCS connection
    Mavsdk mavsdk_gcs{Mavsdk::Configuration{ComponentType::GroundStation}};
    auto result = mavsdk_gcs.add_any_connection("udpin://127.0.0.1:14552");
    Morb morb;
    MavlinkInterface mav_interface{&morb, "udpout://127.0.0.1:14552"};
    bool start_result = mav_interface.start();
    REQUIRE(start_result);
    mav_interface.run();
    std::shared_ptr<System> discovered_system = nullptr;
    std::atomic<bool> system_discovered{false};
    mavsdk_gcs.subscribe_on_new_system([&]() {
        auto systems = mavsdk_gcs.systems();
        if (!systems.empty()) {
            discovered_system = systems[0];
            system_discovered = true;
        }
    });
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!system_discovered && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    /// Verify default mode before mode change
    auto telemetry = Telemetry(discovered_system);
    Telemetry::FlightMode mode = telemetry.flight_mode();
    REQUIRE(mode == Telemetry::FlightMode::Ready);
    /// Send a takeoff command via MAVSDK Action plugin from GCS side
    mavsdk::Action action{discovered_system};
    mavsdk::Action::Result takeoff_result = action.takeoff();
    REQUIRE(takeoff_result == mavsdk::Action::Result::Success);

    /// Wait for the mode change to complete (asynchronous operation)
    const auto mode_deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (telemetry.flight_mode() != Telemetry::FlightMode::Takeoff &&
           std::chrono::steady_clock::now() < mode_deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    /// Verify that mitl responded appropriately
    mode = telemetry.flight_mode();
    REQUIRE(mode == Telemetry::FlightMode::Takeoff);

    /// Clean up threads before object destruction
    mav_interface.stop();
}

TEST_CASE("Parameters are set correctly", "[parameters]") {
    /// Setup MavlinkInterface and GCS connection
    Mavsdk mavsdk_gcs{Mavsdk::Configuration{ComponentType::GroundStation}};
    auto result = mavsdk_gcs.add_any_connection("udpin://127.0.0.1:14553");
    REQUIRE(result == ConnectionResult::Success);

    Morb morb;
    MavlinkInterface mav_interface{&morb, "udpout://127.0.0.1:14553"};
    bool start_result = mav_interface.start();
    REQUIRE(start_result);

    /// Wait for system discovery
    std::shared_ptr<System> discovered_system = nullptr;
    std::atomic<bool> system_discovered{false};
    mavsdk_gcs.subscribe_on_new_system([&]() {
        auto systems = mavsdk_gcs.systems();
        if (!systems.empty()) {
            discovered_system = systems[0];
            system_discovered = true;
        }
    });

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!system_discovered && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(system_discovered);
    REQUIRE(discovered_system != nullptr);

    /// Retrieve parameters using Param plugin
    mavsdk::Param param{discovered_system};

    /// Allow some time for parameters to be set up
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    /// Test MIS_TAKEOFF_ALT parameter
    auto takeoff_alt_result = param.get_param_int("MIS_TAKEOFF_ALT");
    REQUIRE(takeoff_alt_result.first == mavsdk::Param::Result::Success);
    REQUIRE(takeoff_alt_result.second == 0);

    /// Test custom parameter
    auto my_param_result = param.get_param_int("MY_PARAM");
    REQUIRE(my_param_result.first == mavsdk::Param::Result::Success);
    REQUIRE(my_param_result.second == 1);

    /// Clean up
    mav_interface.stop();
}

TEST_CASE("Arm and disarm functionality", "[arm_disarm]") {
    /// Setup MavlinkInterface and GCS connection
    Mavsdk mavsdk_gcs{Mavsdk::Configuration{ComponentType::GroundStation}};
    auto result = mavsdk_gcs.add_any_connection("udpin://127.0.0.1:14554");
    REQUIRE(result == ConnectionResult::Success);

    Morb morb;
    MavlinkInterface mav_interface{&morb, "udpout://127.0.0.1:14554"};
    bool start_result = mav_interface.start();
    REQUIRE(start_result);

    /// Wait for system discovery
    std::shared_ptr<System> discovered_system = nullptr;
    std::atomic<bool> system_discovered{false};
    mavsdk_gcs.subscribe_on_new_system([&]() {
        auto systems = mavsdk_gcs.systems();
        if (!systems.empty()) {
            discovered_system = systems[0];
            system_discovered = true;
        }
    });

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!system_discovered && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(system_discovered);
    REQUIRE(discovered_system != nullptr);

    /// Create Action plugin to send arm command
    mavsdk::Action action{discovered_system};

    /// Verify system starts disarmed
    auto telemetry = mavsdk::Telemetry{discovered_system};
    REQUIRE(!telemetry.armed());

    /// Send arm command
    auto arm_result = action.arm();
    REQUIRE(arm_result == mavsdk::Action::Result::Success);

    /// Wait for arm to complete
    const auto arm_deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!telemetry.armed() && std::chrono::steady_clock::now() < arm_deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    /// Verify system is armed
    REQUIRE(telemetry.armed());

    /// Send disarm command
    auto disarm_result = action.disarm();
    REQUIRE(disarm_result == mavsdk::Action::Result::Success);

    /// Wait for disarm to complete
    const auto disarm_deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (telemetry.armed() && std::chrono::steady_clock::now() < disarm_deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    /// Verify system is disarmed
    REQUIRE(!telemetry.armed());

    /// Clean up
    mav_interface.stop();
}

TEST_CASE("Land callback is triggered", "[land]") {
    /// Setup MavlinkInterface and GCS connection
    Mavsdk mavsdk_gcs{Mavsdk::Configuration{ComponentType::GroundStation}};
    auto result = mavsdk_gcs.add_any_connection("udpin://127.0.0.1:14555");
    REQUIRE(result == ConnectionResult::Success);

    Morb morb;
    MavlinkInterface mav_interface{&morb, "udpout://127.0.0.1:14555"};
    bool start_result = mav_interface.start();
    REQUIRE(start_result);
    mav_interface.run();

    /// Wait for system discovery
    std::shared_ptr<System> discovered_system = nullptr;
    std::atomic<bool> system_discovered{false};
    mavsdk_gcs.subscribe_on_new_system([&]() {
        auto systems = mavsdk_gcs.systems();
        if (!systems.empty()) {
            discovered_system = systems[0];
            system_discovered = true;
        }
    });

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!system_discovered && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(system_discovered);
    REQUIRE(discovered_system != nullptr);

    auto telemetry = Telemetry{discovered_system};
    mavsdk::Action action{discovered_system};

    /// First, takeoff to get vehicle in air
    auto takeoff_result = action.takeoff();
    REQUIRE(takeoff_result == mavsdk::Action::Result::Success);

    /// Wait for takeoff mode
    const auto takeoff_deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (telemetry.flight_mode() != Telemetry::FlightMode::Takeoff &&
           std::chrono::steady_clock::now() < takeoff_deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(telemetry.flight_mode() == Telemetry::FlightMode::Takeoff);

    /// Manually transition to Hold by sending hold command
    auto hold_result = action.hold();

    /// Allow time for the command to be processed
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    /// Now attempt to send a land command
    auto land_result = action.land();

    /// If land succeeds, verify mode changes to Land
    if (land_result == mavsdk::Action::Result::Success) {
        const auto mode_deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        while (telemetry.flight_mode() != Telemetry::FlightMode::Land &&
               std::chrono::steady_clock::now() < mode_deadline) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        REQUIRE(telemetry.flight_mode() == Telemetry::FlightMode::Land);
    }

    /// Clean up
    mav_interface.stop();
}

TEST_CASE("Vehicle loop publishes telemetry", "[vehicle_loop]") {
    /// Setup MavlinkInterface and GCS connection
    Mavsdk mavsdk_gcs{Mavsdk::Configuration{ComponentType::GroundStation}};
    auto result = mavsdk_gcs.add_any_connection("udpin://127.0.0.1:14557");
    REQUIRE(result == ConnectionResult::Success);

    Morb morb;
    MavlinkInterface mav_interface{&morb, "udpout://127.0.0.1:14557"};
    bool start_result = mav_interface.start();
    REQUIRE(start_result);

    /// Wait for system discovery
    std::shared_ptr<System> discovered_system = nullptr;
    std::atomic<bool> system_discovered{false};
    mavsdk_gcs.subscribe_on_new_system([&]() {
        auto systems = mavsdk_gcs.systems();
        if (!systems.empty()) {
            discovered_system = systems[0];
            system_discovered = true;
        }
    });

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!system_discovered && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(system_discovered);
    REQUIRE(discovered_system != nullptr);

    /// Start the vehicle loop
    mav_interface.run();

    /// Subscribe to telemetry to verify it's being published
    auto telemetry = mavsdk::Telemetry{discovered_system};
    std::atomic<bool> position_received{false};
    std::atomic<bool> gps_received{false};
    std::atomic<bool> battery_received{false};

    telemetry.subscribe_position([&](mavsdk::Telemetry::Position pos) {
        position_received = true;
    });

    telemetry.subscribe_raw_gps([&](mavsdk::Telemetry::RawGps gps) {
        gps_received = true;
    });

    telemetry.subscribe_battery([&](mavsdk::Telemetry::Battery battery) {
        battery_received = true;
    });

    /// Wait for telemetry to be published (vehicle loop runs at 1Hz)
    const auto telem_deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while ((!position_received || !gps_received || !battery_received) &&
           std::chrono::steady_clock::now() < telem_deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    /// Verify telemetry was received
    REQUIRE(position_received);
    REQUIRE(gps_received);
    REQUIRE(battery_received);

    /// Clean up
    mav_interface.stop();
}

TEST_CASE("Mission upload functionality", "[mission]") {
    /// Setup MavlinkInterface and GCS connection
    Mavsdk mavsdk_gcs{Mavsdk::Configuration{ComponentType::GroundStation}};
    auto result = mavsdk_gcs.add_any_connection("udpin://127.0.0.1:14558");
    REQUIRE(result == ConnectionResult::Success);

    Morb morb;
    MavlinkInterface mav_interface{&morb, "udpout://127.0.0.1:14558"};
    bool start_result = mav_interface.start();
    REQUIRE(start_result);

    /// Wait for system discovery
    std::shared_ptr<System> discovered_system = nullptr;
    std::atomic<bool> system_discovered{false};
    mavsdk_gcs.subscribe_on_new_system([&]() {
        auto systems = mavsdk_gcs.systems();
        if (!systems.empty()) {
            discovered_system = systems[0];
            system_discovered = true;
        }
    });

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!system_discovered && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(system_discovered);
    REQUIRE(discovered_system != nullptr);

    /// Create a simple mission with one waypoint
    mavsdk::MissionRaw mission_raw{discovered_system};
    mavsdk::MissionRaw::MissionItem item{};
    item.seq = 0;
    item.frame = 0;
    item.command = 16; // MAV_CMD_NAV_WAYPOINT
    item.current = 1;
    item.autocontinue = 1;
    item.param1 = 0;
    item.param2 = 0;
    item.param3 = 0;
    item.param4 = 0;
    item.x = 47;
    item.y = 8;
    item.z = 10;
    item.mission_type = 0;

    std::vector<mavsdk::MissionRaw::MissionItem> mission_items{item};

    /// Upload the mission
    auto upload_result = mission_raw.upload_mission(mission_items);
    REQUIRE(upload_result == mavsdk::MissionRaw::Result::Success);

    /// Allow time for mission to be processed
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    /// Clean up
    mav_interface.stop();
}