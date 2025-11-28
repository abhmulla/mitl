/**
 * @file active.h
 * @author Abdulelah Mulla
 * @date 11/28/2025
 */

#pragma once

#include "mode/mode.h"

class Active : public Mode {
public:
    Active();

    void on_activation() override;

    void on_active() override;

    void on_inactivation() override;

    void on_inactive() override;

    bool is_complete() const override;
};