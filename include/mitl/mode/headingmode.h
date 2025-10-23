/**
 * @file headingmode.h
 * @author Abdulelah Mulla
 * @brief Header file for the heading mode class
 * @version 0.1
 * @date 2025-10-22
 */

#pragma once

#include "mode.h"

/**
 * @brief Heading mode - vehicle flies to a waypoint
 */
class HeadingMode : public Mode {
public:
    HeadingMode(Vehicle& vehicle) : Mode(vehicle) {}
    ModeType get_type() const override;
    void enter() override;
    void exit() override;
    void update() override;
    bool is_complete() override;
};
