/**
 * @file takeoff.h
 * @author Abdulelah Mulla
 * @brief Header file for the takeoff mode
 * @version 0.1
 * @date 2025-11-23
 */

#pragma once

#include "mode.h"

/**
 * @brief Executes takeoff logic and updates position.
 */
class Takeoff: public Mode {
public:
    void on_activation() override;
    void on_active() override;
};