#include "Pass_filter.h"

Pass_filter::Pass_filter(Controller * controller){
    Filter::controller = controller;
}

void Pass_filter::recieve_pos(uint16_t pos_uint){
    
}

void Pass_filter::recieve_angle(double phi){
    Filter::state[0] = phi;
    Filter::publish();
}

void Pass_filter::recieve_velo(double velo){
    Filter::state[1] = velo;
    Filter::publish();
}