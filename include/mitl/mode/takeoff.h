/**
 * @file takeoff.h
 * @author Abdulelah Mulla
 * @brief Header file for the takeoff mode
 * @version 0.1
 * @date 2025-11-23
 */

#pragma once

#include "mode.h"
#include "position.h"

class Navigator;
class Morb;

/**
 * @brief Enum defining current takeoff state
 */
enum class TakeoffState {
    INIT,
    CLIMBING,
    COMPLETE
};

/**
 * @brief Executes takeoff logic and updates position.
 */
class Takeoff: public Mode {
private:
    Morb *_morb;
    Navigator *_navigator;

    float _takeoff_alt_amsl{0};  // Target altitude AMSL

    bool _active{false};
    
    /// State tracking
    TakeoffState _state{TakeoffState::INIT};

    /// Completion threshold
    const float ALTITUDE_THRESHOLD = 0.5f;  // meters
public:
    /// Constructor and destructor
    Takeoff(Morb *morb, Navigator *navigator);
    ~Takeoff();

    /// Disable default constructor
    Takeoff() = delete;
    /// Disable Assignment operator
    Takeoff& operator=(const Takeoff&) = delete;

    /**
     * @brief Set the initial position and parameters for takeoff.
     */
    void on_activation() override;

    /**
     * @brief Update positions for takeoff
     */
    void on_active() override;
    
    /**
     * @brief Switches mode to INIT
     */
    void on_inactivation() override;

    void on_inactive() override;

    /// Check if takeoff is complete
    bool is_complete() const override;
};