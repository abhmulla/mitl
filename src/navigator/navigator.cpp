/**
 * @file navigator.cpp
 * @author Abdulelah Mulla
 */

#include "navigator/navigator.h"

void Navigator::run() {
    /// Iterate through mode list and set appropriately
    for (int i = 0; i < MODE_ARRAY_SIZE; i++) {
        if(_modes[i]) {
            _modes[i]->run(_curr_mode == _modes[i]);
        }
    }
    /// Publish updates
}