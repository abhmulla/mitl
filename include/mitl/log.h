/**
 * @file log.h
 * @author Abdulelah Mulla
 * @brief Shared log file
 * @version 0.2
 * @date 2026-01-01
 */

#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

/**
 * @brief Class for managing logging with thread safety.
 */
class MITL_LOG {
private:
    std::ofstream _program_log; // Logs the start of different processes

    /// Mutex for accessing shared Log files
    std::mutex _program_mutex; 

    /**
     * Constructor
     * @brief Defines the log files.
     * The constructor is private because we are using a singleton pattern approach.
     */
    MITL_LOG();
public:
    /**
     * @brief Initialize a single instance of the Log.
     */
    static MITL_LOG& initialize();

    /**
     * Destructor
     * @brief Closes files.
     */
    ~MITL_LOG();

    /// Delete copy constructor and assignment operator
    MITL_LOG(const MITL_LOG&) = delete;
    MITL_LOG operator=(const MITL_LOG&) = delete;
    
    /**
     * @brief Safely logs the message in the program_log file.
     */
    void program_log(const std::string &msg);
};