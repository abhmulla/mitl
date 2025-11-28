/**
 * @file hold.cpp
 * @author Abdulelah Mulla
 */

#include "mode/hold.h"

Hold::Hold() {
    state_id = 1;
}

void Hold::on_activation() {

}

void Hold::on_active() {

}

void Hold::on_inactivation() {

}

void Hold::on_inactive() {

}

bool Hold::is_complete() const {
    return false;
}