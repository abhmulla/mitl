/**
 * @file mode.cpp
 * @author Abdulelah Mulla
 */

#include "mode/mode.h"

void Mode::run(bool active) {
    if(active) {
        if(!_active) {
            on_activation();
        } else {
            on_active();
        }
    } else {
        if(_active) {
            on_inactivation();
        } else {
            on_inactive();
        }
    }
    _active = active;
}