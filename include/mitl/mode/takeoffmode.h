/**
 * @file takeoffmode.h
 * @author Abdulelah Mulla
 * @brief Header file for the take off mode class
 * @version 0.1
 * @date 2025-10-21
 */

#pragma once

#include "mode.h"

class TakeOffMode : public Mode {
private:

public:
    TakeOffMode(Vehicle& vehicle) : Mode(vehicle) {}
    ModeType get_type() const override;
    void enter() override;
    void exit() override;
    void update() override;
    bool is_complete() override;
};