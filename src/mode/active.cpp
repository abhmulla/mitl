/**
 * @file active.cpp
 * @author Abdulelah Mulla
 */

#include "mode/active.h"

Active::Active() {
    state_id = 0;
}

void Active::on_activation() {

}

void Active::on_active() {

}

void Active::on_inactivation() {

}

void Active::on_inactive() {

}

bool Active::is_complete() const {
    return false;
}