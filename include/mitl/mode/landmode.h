/**
 * @file landmode.h
 * @author Abdulelah Mulla
 * @brief Header file for the land mode class
 * @version 0.1
 * @date 2025-10-22
 */

#pragma once

#include "mode.h"

/**
 * @brief Land mode - vehicle descends and lands
 */
class LandMode : public Mode {
public:
    LandMode(Vehicle& vehicle) : Mode(vehicle) {}
    ModeType get_type() const override;
    void enter() override;
    void exit() override;
    void update() override;
    bool is_complete() override;
};
