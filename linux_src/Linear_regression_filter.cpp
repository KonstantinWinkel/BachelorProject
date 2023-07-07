#include "Linear_regression_filter.h"
#include <chrono>

Linear_regression_filter::Linear_regression_filter(Controller * controller,double camera_latency, double lidar_latencysize_t,size_t size_x, size_t size_phi)
    :lr_x(size_x),lr_phi(size_phi){
    Filter::state = {0,0,0,0};
    Filter::controller = controller;
    Linear_regression_filter::camera_latency = camera_latency;
    Linear_regression_filter::lidar_latency = lidar_latency;
    Linear_regression_filter::start_time = std::chrono::high_resolution_clock::now();
    Linear_regression_filter::time_now = time(start_time);
}

double Linear_regression_filter::time(std::chrono::_V2::system_clock::time_point time){
    return (time-start_time).count()*1000;
}

inline void Linear_regression_filter::update_pos(){
    state[0] = lr_x.get_intercept() + (time_now+lidar_latency)*state[1];
}

inline void Linear_regression_filter::update_angle(){
    state[2] = lr_phi.get_intercept() + (time_now+camera_latency)*state[1];
}

void Linear_regression_filter::recieve_pos(uint16_t pos_uint){
    time_now = time(std::chrono::high_resolution_clock::now());
    lr_x.set(datapoint{to_m(pos_uint),time_now});
    state[1] = lr_x.get_slope();
    update_pos();
    update_angle();
    publish();
}

void Linear_regression_filter::recieve_angle(double phi){
    time_now = time(std::chrono::high_resolution_clock::now());
    lr_x.set(datapoint{to_m(phi),time_now});
    state[3] = lr_phi.get_slope();
    update_pos();
    update_angle();
    publish();
}