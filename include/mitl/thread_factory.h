/**
 * @file thread_factory.h
 * @author Abdulelah Mulla
 * @brief Factory class to manage starting threads.
 * @version 0.1
 * @date 2025-12-29
 */

#pragma once

#include <string>

#include "morb.h"
#include "scheduler.h"
#include "mavlink_interface.h"
#include "gazebo/gazebo_state.h"

/**
 * This class is used to start threads in one place,
 * for simplicity and ensuring threads are started in
 * the correct order.
 */
class ThreadFactory {
private:
    Morb& _morb;
    std::string _world_name;
    std::string _vehicle_name;
public:
    /// Constructor
    ThreadFactory();
    /**
     * @brief Initialize all threads
     */
    void start();
};