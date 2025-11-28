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

public:
    uint8_t state_id{0};
    /**
     * @brief This function tells the mode if it is active or not
     */
    void run(bool active);

    /**
	 * @brief This function is called while the mode is inactive
	 */
	virtual void on_inactive() = 0;

	/**
	 * @brief This function is called one time when mode becomes active, pos_sp_triplet must be initialized here
	 */
	virtual void on_activation() = 0;

	/**
	 * @brief This function is called one time when mode becomes inactive
	 */
	virtual void on_inactivation() = 0;

	/**
	 * @brief This function is called while the mode is active
	 */
	virtual void on_active() = 0;

	/**
	 * @brief Is this mode done?
	 */
	virtual bool is_complete() const = 0;
};