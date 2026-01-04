/**
 * @file log.cpp
 * @author Abdulelah Mulla
 * @brief Definition of the shared log file
 * @version 0.2
 * @date 2026-01-01
 */

#include "log.h"

/**
 * Constructor
 */
MITL_LOG::MITL_LOG() {
    _program_log.open("program_log");
    _sensor_log.open("sensor_log");
}

/**
 * Initialize
 */
MITL_LOG& MITL_LOG::initialize() {
    static MITL_LOG mitl_log; // one instance
    return mitl_log;
}

/**
 * Destructor
 */
MITL_LOG::~MITL_LOG() {
    _program_log.close();
    _sensor_log.close();
}

void MITL_LOG::program_log(const std::string &msg) {
    /// Lock mutex
    const std::lock_guard<std::mutex> lock(_program_mutex);
    /// Log the message
    _program_log << msg << std::endl;
}

void MITL_LOG::sensor_log(const google::protobuf::Message& msg, std::string label, uint64_t time) {
    /// Lock mutex
    const std::lock_guard<std::mutex> lock(_sensor_mutex);
    /// Log the message
    _sensor_log << label << ", Time: " << time << ", Message: " << msg.DebugString() << std::endl;
}