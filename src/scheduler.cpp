/**
 * @file scheduler.cpp
 * @author Abdulelah Mulla
 * 
 * Portions derived from PX4, Copyright (c) 2020 PX4 Development Team
 * Used under the BSD 3-Clause license.
 */

#include "scheduler.h"
#include "log.h"

/**
 * Constructor
 */
Scheduler::Scheduler() {
    mitl_log << "[Scheduler] Initialzed Scheduler" << std::endl;
}

/**
 * Destructor
 */
Scheduler::~Scheduler() {
    const std::lock_guard<std::mutex> lock(_alarms_mutex);
    while (_alarms) {
        Alarm* temp = _alarms;
        _alarms = _alarms->next;
        temp->removed = true;
    }
    mitl_log << "[Scheduler] Destroyed Scheduler" << std::endl;
}

Scheduler& Scheduler::initialize() {
    static Scheduler scheduler; // one instance
    return scheduler;
}

void Scheduler::set_time(uint64_t time_mus) {
    if (_time_mus == 0 && time_mus > 0) {
        mitl_log << "[Scheduler] starting at time: " << time_mus << std::endl;
    }
    _time_mus = time_mus;
    {
        /// Lock linked list mutex
        const std::lock_guard<std::mutex> lock(_alarms_mutex);
        _setting_time = true;
        /// Check for interval satisfaction
        Alarm* alarm = _alarms;
        Alarm* prev = nullptr;
        while(alarm) {
            /// Clean up the ones that are already done from last iteration.
            if(alarm->done) {
                if (prev) {
                    prev->next = alarm->next;
                } else {
                    _alarms = _alarms->next;
                }
                Alarm* temp = alarm;
                alarm = alarm->next;
                temp->removed = true;
                continue;
            }
            if(_time_mus >= alarm->time && !alarm->timeout) {
                /// Notify the condition variable
                pthread_mutex_lock(alarm->mutex);
                alarm->timeout = true;
                pthread_cond_broadcast(alarm->condition_var);
                pthread_mutex_unlock(alarm->mutex);
            }
            prev = alarm;
            alarm = alarm->next;
        }
        _setting_time = false;
    }
}

int Scheduler::cond_timedwait(pthread_cond_t *sleep_cond, pthread_mutex_t *sleep_mutex, uint64_t wakeup_time) {
    static thread_local Alarm alarm;
    {
        const std::lock_guard<std::mutex> lock(_alarms_mutex);
        if (_time_mus >= wakeup_time) {
            return TIMEDOUT;
        }
        /// Assign variables to alarm object
        alarm.time = wakeup_time;
        alarm.condition_var = sleep_cond;
        alarm.mutex = sleep_mutex;
        alarm.done = false;
        if (alarm.removed) {
            /// Add alarm object to the alarms list
            alarm.removed = false;
            alarm.next = _alarms;
            _alarms = &alarm;
        }
    }
    /// Sleep
    int result = pthread_cond_wait(sleep_cond, sleep_mutex);

    bool timeout = alarm.timeout;
    if (result == 0 && timeout) {
        result = TIMEDOUT;
    }

    alarm.done = true;
    return result;
}

int Scheduler::sleep(uint64_t interval) {
    /// Calculate the time till wake up
    uint64_t wakeup_time = _time_mus + interval;

    /// Create condition variable and mutex
    pthread_cond_t sleep_cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t sleep_mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&sleep_mutex);
    int result = cond_timedwait(&sleep_cond, &sleep_mutex, wakeup_time);

    if (result == TIMEDOUT) {
        result = 1;
    }

    pthread_mutex_unlock(&sleep_mutex);
    return result;
}