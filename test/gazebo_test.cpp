/**
 * @file gazebo_test.cpp
 * @author Abdulelah Mulla
 * @brief Tests the build of gazebo
 */

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>
#include <gz/msgs.hh>
#include <gz/transport.hh>

 
static std::atomic<bool> g_terminatePub(false);
 
void signal_handler(int _signal)
{
  if (_signal == SIGINT || _signal == SIGTERM)
    g_terminatePub = true;
}

int main() {
    gz::transport::Node node;
    std::string topic = "/foo";
    auto pub = node.Advertise<gz::msgs::StringMsg>(topic);
    if (!pub) {
        std::cerr << "Error advertising topic [" << topic << "]" << std::endl;
        return -1;
    }
    gz::msgs::StringMsg msg;
    msg.set_data("HELLO");
    
    // Publish messages at 1Hz.
    while (!g_terminatePub)
    {
        if (!pub.Publish(msg))
        break;
    
        std::cout << "Publishing hello on topic [" << topic << "]" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    return 0;
}