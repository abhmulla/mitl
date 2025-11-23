/**
 * @file mode.h
 * @author Abdulelah Mulla
 * @brief Header file for the base mode
 * @version 0.1
 * @date 2025-11-23
 */

#pragma once

#include <cstdint>

/**
 * @brief base class for modes
 */
class Mode {
private:
    bool _active{false};
    uint8_t _state_id{0};
public:
    /**
	 * This function is called while the mode is inactive
	 */
	virtual void on_inactive();

	/**
	 * This function is called one time when mode becomes active, pos_sp_triplet must be initialized here
	 */
	virtual void on_activation();

	/**
	 * This function is called one time when mode becomes inactive
	 */
	virtual void on_inactivation();

	/**
	 * This function is called while the mode is active
	 */
	virtual void on_active();
};