/**
 * @file scheduler.h
 * @author Abdulelah Mulla
 * @brief About Time.
 * @version 0.1
 * @date 2025-12-23
 */

#pragma once

#define TIMEDOUT 2

#include <cstdint>
#include <mutex>
#include <pthread.h>
#include <atomic>

/**
 * @brief Keeps time and provides it to those who need it.
 * This class is responsible for storing the time which is 
 * needed by various components. Additionally, it also wakes
 * up sleeping threads that need time to run.
 */
class Scheduler {
private:
    std::atomic<uint64_t> _time_mus{0}; // in µs

    /**
     * @brief Alarm object that will be used in sleep.
     * This pobject is used to keep track of sleeping threads,
     * and when we need to wake them up. We keep track of when
     * they need to wake up by calculating the time given the interval
     * in the sleep function, and we wake the them up by creating condition
     * variables.
     */
    struct Alarm
    {
        uint64_t time{0};
        pthread_cond_t *condition_var{nullptr};
		pthread_mutex_t *mutex{nullptr};
        Alarm* next{nullptr};
        std::atomic<bool> done{false};
        std::atomic<bool> removed{true};
        std::atomic<bool> timeout{false};
    };
    
    Alarm* _alarms = nullptr; // head of linked list
    std::mutex _alarms_mutex;

    bool _setting_time{false};

    int cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *lock, uint64_t time_us);

    /// Constructor
    Scheduler();
public:
    /// Delete copy constructor and assignment operator
    Scheduler(const Scheduler&) = delete;
    Scheduler operator=(const Scheduler&) = delete;

    /// Desctructor
    ~Scheduler();

    /**
     * Singleton approach to give global access to this class.
     */
    static Scheduler& initialize();

    /**
     * @brief Get the current time in µs.
     * @return The current time in µs.
     */
    uint64_t get_time() {return _time_mus;}

    /**
     * @brief Update the time variable.
     * @param time_mus time in microseconds
     * This function is updated in each clock callback
     * from Gazebo.
     */
    void set_time(uint64_t time_mus);

    /**
     * @brief Put the running thread to sleep until the interval.
     * @param interval time interval until wake up, in microseconds.
     */
    int sleep(uint64_t interval);
};