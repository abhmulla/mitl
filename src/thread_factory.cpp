/**
 * @file thread_factory.cpp
 * @author Abdulelah Mulla
 */

#include "thread_factory.h"
#include "scheduler.h"


/**
 * Constructor
 * @brief Create instances of the classes required
 * to start a thread for a functionality.
 */
ThreadFactory::ThreadFactory() {

}

/**
 * We only start a few for now...
 * Will integrate fully after the scheduler is tested and added.
 */
void ThreadFactory::start() {
    Scheduler::initialize();
}