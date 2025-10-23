/**
 * @file holdmode.h
 * @author Abdulelah Mulla
 * @brief Header file for the hold mode class
 * @version 0.1
 * @date 2025-10-22
 */

#pragma once

#include "mode.h"

/**
 * @brief Hold mode - vehicle maintains position in air
 */
class HoldMode : public Mode {
public:
    HoldMode(Vehicle& vehicle) : Mode(vehicle) {}
    ModeType get_type() const override;
    void enter() override;
    void exit() override;
    void update() override;
    bool is_complete() override;
};
