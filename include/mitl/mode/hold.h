/**
 * @file hold.h
 * @author Abdulelah Mulla
 * @date 11/25/2025
 */

#pragma once

#include "mode/mode.h"

/// TODO:
class Hold : public Mode {
public:
    Hold();

    void on_activation() override;

    void on_active() override;

    void on_inactivation() override;

    void on_inactive() override;

    bool is_complete() const override;
};