/**
 * @file groundmode.h
 * @author Abdulelah Mulla
 * @brief Header file for the ground mode class
 * @version 0.1
 * @date 2025-10-22
 */

#pragma once

#include "mode.h"

/**
 * @brief Ground mode - vehicle is on the ground
 */
class GroundMode : public Mode {
public:
    GroundMode(Vehicle& vehicle) : Mode(vehicle) {}
    ModeType get_type() const override;
    void enter() override;
    void exit() override;
    void update() override;
    bool is_complete() override;
};
