/**
 * @file gazebo_test.cpp
 * @author Abdulelah Mulla
 * @brief Tests the build of gazebo
 * @version 0.1
 * @date 2025-12-14
 */

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

#include <gz/msgs.hh>
#include <gz/transport.hh>
#include <catch2/catch_test_macros.hpp>


TEST_CASE ("Strings are published", "[publish]") {
    gz::transport::Node node;

    const std::string topic = "/mitl_test";

    std::atomic<bool> received{false};
    std::string receivedData;

    /// Create subscriber and test it
    std::function<void(const gz::msgs::StringMsg &)> cb = 
        [&](const gz::msgs::StringMsg &msg)
        {
            receivedData = msg.data();
            received = true;
        };

    bool sub = node.Subscribe(topic, cb);
    REQUIRE(sub);
    
    /// Advertise and test
    auto pub = node.Advertise<gz::msgs::StringMsg>(topic);
    REQUIRE(pub);

    /// Publish and test
    gz::msgs::StringMsg out;
    out.set_data("HELLO");
    REQUIRE(pub.Publish(out));
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!received && std::chrono::steady_clock::now() < deadline)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    REQUIRE(received);
    REQUIRE(receivedData == "HELLO");
}