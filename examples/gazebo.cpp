/**
 * @file gazebo.cpp
 * @author Abdulelah Mulla
 * @brief demonstration of gazebo_state subscriptions
 * */

#include <string>
#include <chrono>
#include "gazebo/gazebo_state.h"
#include "morb.h"

int main() {
    std::string world = "default", vehicle = "x500_0";
    Morb morb;
    GazeboState gazebo_state(&morb, world, vehicle);
    gazebo_state.activate_subscriptions();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 0;
}