/**
 * @file mode_manager_test.cpp
 * @author Abdulelah Mulla
 * @brief Unit tests for the mode manager
 * @version 0.1
 * @date 2025-12-17
 */

#include "mode_manager.h"
#include "vehicle.h"
#include "morb.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/server_component.h>
#include <mavsdk/plugins/action_server/action_server.h>
#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>
#include <thread>
#include <memory>

TEST_CASE("ModeManager valid state transitions", "[mode_manager]") {
    /// Setup MAVSDK autopilot side
    mavsdk::Mavsdk mavsdk_autopilot{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::Autopilot}};
    auto connection_result = mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:14560");
    REQUIRE(connection_result == mavsdk::ConnectionResult::Success);

    auto server = mavsdk_autopilot.server_component();
    REQUIRE(server != nullptr);

    /// Setup GCS side to verify mode changes
    mavsdk::Mavsdk mavsdk_gcs{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::GroundStation}};
    auto gcs_result = mavsdk_gcs.add_any_connection("udpin://127.0.0.1:14560");
    REQUIRE(gcs_result == mavsdk::ConnectionResult::Success);

    /// Wait for GCS to discover the system
    std::atomic<bool> system_discovered{false};
    std::shared_ptr<mavsdk::System> discovered_system = nullptr;
    mavsdk_gcs.subscribe_on_new_system([&]() {
        auto systems = mavsdk_gcs.systems();
        if (!systems.empty()) {
            discovered_system = systems[0];
            system_discovered = true;
        }
    });

    /// Wait for system discovery
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!system_discovered && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(system_discovered);
    REQUIRE(discovered_system != nullptr);

    /// Initialize components
    Morb morb;
    mavsdk::ActionServer action{server};
    Vehicle vehicle(server, discovered_system, &morb);
    ModeManager mode_manager(vehicle, action, &morb);

    /// Initialize and start mode manager
    mode_manager.initialize_modes();
    mode_manager.start();

    /// Initial mode should be Ready
    auto mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Ready);

    /// Ready to Takeoff
    bool result = mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Takeoff);
    REQUIRE(result);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Takeoff);

    /// Takeoff to Hold
    result = mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Hold);
    REQUIRE(result);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Hold);

    /// Hold to Mission
    result = mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Mission);
    REQUIRE(result);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Mission);

    /// Mission to Hold
    result = mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Hold);
    REQUIRE(result);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Hold);

    /// Hold to Land
    result = mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Land);
    REQUIRE(result);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Land);

    /// Land to Ready
    result = mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Ready);
    REQUIRE(result);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Ready);

    /// Clean up
    mode_manager.stop();
}

TEST_CASE("ModeManager invalid state transitions", "[mode_manager]") {
    /// Setup MAVSDK autopilot side
    mavsdk::Mavsdk mavsdk_autopilot{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::Autopilot}};
    auto connection_result = mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:14561");
    REQUIRE(connection_result == mavsdk::ConnectionResult::Success);

    auto server = mavsdk_autopilot.server_component();
    REQUIRE(server != nullptr);

    /// Setup GCS side to verify mode changes
    mavsdk::Mavsdk mavsdk_gcs{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::GroundStation}};
    auto gcs_result = mavsdk_gcs.add_any_connection("udpin://127.0.0.1:14561");
    REQUIRE(gcs_result == mavsdk::ConnectionResult::Success);

    /// Wait for GCS to discover the system
    std::atomic<bool> system_discovered{false};
    std::shared_ptr<mavsdk::System> discovered_system = nullptr;
    mavsdk_gcs.subscribe_on_new_system([&]() {
        auto systems = mavsdk_gcs.systems();
        if (!systems.empty()) {
            discovered_system = systems[0];
            system_discovered = true;
        }
    });

    /// Wait for system discovery
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!system_discovered && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(system_discovered);
    REQUIRE(discovered_system != nullptr);

    /// Initialize components
    Morb morb;
    mavsdk::ActionServer action{server};
    Vehicle vehicle(server, discovered_system, &morb);
    ModeManager mode_manager(vehicle, action, &morb);

    /// Initialize and start mode manager
    mode_manager.initialize_modes();
    mode_manager.start();

    /// Ready to Land
    auto mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Ready);
    bool result = mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Land);
    REQUIRE(!result);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Ready); /// Mode should not change

    /// Ready to Hold
    result = mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Hold);
    REQUIRE(!result);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Ready);

    /// Ready to Mission
    result = mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Mission);
    REQUIRE(!result);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Ready);

    result = mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Takeoff);
    REQUIRE(result);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    /// Takeoff to Ready
    result = mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Ready);
    REQUIRE(!result);
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Takeoff);

    /// Takeoff to Land
    result = mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Land);
    REQUIRE(!result);
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Takeoff);

    /// Takeoff to Mission
    result = mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Mission);
    REQUIRE(!result);
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Takeoff);

    /// Clean up
    mode_manager.stop();
}

TEST_CASE("ModeManager activate_takeoff method", "[mode_manager]") {
    /// Setup MAVSDK autopilot side
    mavsdk::Mavsdk mavsdk_autopilot{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::Autopilot}};
    auto connection_result = mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:14562");
    REQUIRE(connection_result == mavsdk::ConnectionResult::Success);

    auto server = mavsdk_autopilot.server_component();
    REQUIRE(server != nullptr);

    /// Setup GCS side to verify mode changes
    mavsdk::Mavsdk mavsdk_gcs{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::GroundStation}};
    auto gcs_result = mavsdk_gcs.add_any_connection("udpin://127.0.0.1:14562");
    REQUIRE(gcs_result == mavsdk::ConnectionResult::Success);

    /// Wait for GCS to discover the system
    std::atomic<bool> system_discovered{false};
    std::shared_ptr<mavsdk::System> discovered_system = nullptr;
    mavsdk_gcs.subscribe_on_new_system([&]() {
        auto systems = mavsdk_gcs.systems();
        if (!systems.empty()) {
            discovered_system = systems[0];
            system_discovered = true;
        }
    });

    /// Wait for system discovery
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!system_discovered && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(system_discovered);
    REQUIRE(discovered_system != nullptr);

    /// Initialize components
    Morb morb;
    mavsdk::ActionServer action{server};
    Vehicle vehicle(server, discovered_system, &morb);
    ModeManager mode_manager(vehicle, action, &morb);

    /// Initialize and start mode manager
    mode_manager.initialize_modes();
    mode_manager.start();

    /// Verify initial mode is Ready
    auto mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Ready);

    /// Test activate_takeoff
    mode_manager.activate_takeoff();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Takeoff);

    /// Clean up
    mode_manager.stop();
}

TEST_CASE("ModeManager activate_land method", "[mode_manager]") {
    /// Setup MAVSDK autopilot side
    mavsdk::Mavsdk mavsdk_autopilot{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::Autopilot}};
    auto connection_result = mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:14563");
    REQUIRE(connection_result == mavsdk::ConnectionResult::Success);

    auto server = mavsdk_autopilot.server_component();
    REQUIRE(server != nullptr);

    /// Setup GCS side to verify mode changes
    mavsdk::Mavsdk mavsdk_gcs{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::GroundStation}};
    auto gcs_result = mavsdk_gcs.add_any_connection("udpin://127.0.0.1:14563");
    REQUIRE(gcs_result == mavsdk::ConnectionResult::Success);

    /// Wait for GCS to discover the system
    std::atomic<bool> system_discovered{false};
    std::shared_ptr<mavsdk::System> discovered_system = nullptr;
    mavsdk_gcs.subscribe_on_new_system([&]() {
        auto systems = mavsdk_gcs.systems();
        if (!systems.empty()) {
            discovered_system = systems[0];
            system_discovered = true;
        }
    });

    /// Wait for system discovery
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!system_discovered && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(system_discovered);
    REQUIRE(discovered_system != nullptr);

    /// Initialize components
    Morb morb;
    mavsdk::ActionServer action{server};
    Vehicle vehicle(server, discovered_system, &morb);
    ModeManager mode_manager(vehicle, action, &morb);

    /// Initialize and start mode manager
    mode_manager.initialize_modes();
    mode_manager.start();

    /// Get to Hold mode first (via Takeoff)
    mode_manager.activate_takeoff();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mode_manager.change_mode(mavsdk::ActionServer::FlightMode::Hold);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    /// Verify we're in Hold mode
    auto mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Hold);

    /// Test activate_land
    mode_manager.activate_land();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    mode = mode_manager.get_current_mode();
    REQUIRE(mode == mavsdk::ActionServer::FlightMode::Land);

    /// Clean up
    mode_manager.stop();
}