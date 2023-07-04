#include "Filter.h"
#include <iostream>
#include "thread"

#include "bachelor_debug.h"

#if defined(_DEBUG_FILTER_ON_)
#define _debug_print_filter_(x) std::cout << x << std::endl
#else
#define _debug_print_filter_(x)
#endif

#define toradiants 0.01745329251

Filter::Filter(Controller * controller,size_t size_x, size_t size_phi) 
    :RingBuffer_x(size_x,[=](int i){return size_x-i;}),RingBuffer_phi(size_phi,[=](int i){return size_phi-i;}){
    state = {0,0,0,0};
    Filter::bias = 0;
    Filter::controller = controller;
    last_time_x = std::chrono::high_resolution_clock::now();
    current_time_x = std::chrono::high_resolution_clock::now();
    last_time_phi = std::chrono::high_resolution_clock::now();
    current_time_phi = std::chrono::high_resolution_clock::now();
}

inline double Filter::to_radiants(double angle){
    return toradiants*angle;
}

Filter::~Filter(){}

double Filter::to_m(uint16_t lidar_data){
    sum += lidar_data/1000.0-Filter::bias/1000.0;
    no++;
    std::cout << "bias: " << sum/no << std::endl;
    return lidar_data/1000.0-Filter::bias/1000.0;
}

std::string state_str(std::array<double,4> state){
    std::stringstream s;
    for(int i = 0; i<3;i++) s<< state[i] << ',';
    s<<state[3];
    return s.str();
}

void Filter::publish(){
    _debug_print_filter_("published state :" << state_str(state));
    controller->recieve_data(state);
}

void Filter::recieve_pos(uint16_t pos_uint){
    double average_x = 0;
    current_time_x = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_x;
    _debug_print_filter_("x :" << pos_uint);
    RingBuffer_x.set(to_m(pos_uint));
    average_x = RingBuffer_x.get_weighted_average();
    duration_x = current_time_x-last_time_x;
    state[1] = ((average_x-state[0])/duration_x.count())*1000;
    last_time_x = current_time_x;
    state[0] = average_x;
    publish();
}

void Filter::recieve_angle(double phi){
    double average_phi = 0;
    current_time_phi = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_phi;
    //_debug_print_filter_("phi :" << phi);
    RingBuffer_phi.set(to_radiants(phi));
    average_phi = RingBuffer_phi.get_weighted_average();
    duration_phi = current_time_phi-last_time_phi;
    state[3] = ((average_phi-state[2])/duration_phi.count())*1000;
    last_time_phi = current_time_phi;
    state[2] = average_phi;
    publish();
}