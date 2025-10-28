/**
 * @file vehicle.h
 * @author Abdulelah Mulla
 * @brief Header file for the mode base class
 * @version 0.1
 * @date 2025-10-21
 */

#pragma once

/// Forward declaration
class Vehicle;

/**
 * @brief Enums of the possible mode types
 * 
 * Used for quick access of modes
 */
enum class ModeType {
    Ground,
    Takeoff,
    Hold,
    Heading,
    Land
};

/**
 * @brief Base class for the vehicle Mode types
 * 
 * This class presents the functionality that each mode 
 * should perform.
 */
class Mode {
protected:
    Vehicle& vehicle;
public:
    Mode(Vehicle& vehicle) : vehicle(vehicle) {}
    virtual ~Mode() = default;
    virtual ModeType get_type() const = 0;
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update() = 0;
    virtual bool is_complete() = 0;
};