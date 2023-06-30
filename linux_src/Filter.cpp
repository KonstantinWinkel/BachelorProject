#include "Filter.h"
#include <iostream>
#include "thread"

#include "bachelor_debug.h"

#if defined(_DEBUG_FILTER_ON_)
#define _debug_print_filter_(x) std::cout << x << std::endl
#else
#define _debug_print_filter_(x)
#endif

Filter::Filter(Controller * controller,size_t size_x, size_t size_phi) 
    :RingBuffer_x(size_x,[=](int i){return size_x-i;}),RingBuffer_phi(size_phi,[=](int i){return size_phi-i;}){
    Filter::bias = 0;
    last_time_x = std::chrono::high_resolution_clock::now();
    current_time_x = std::chrono::high_resolution_clock::now();
    last_time_phi = std::chrono::high_resolution_clock::now();
    current_time_phi = std::chrono::high_resolution_clock::now();
}

Filter::~Filter(){}

double Filter::to_m(uint16_t lidar_data){
    return (lidar_data-Filter::bias)/1000;
}

void Filter::publish(){
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
    state[0] = average_x;
    publish();
}

void Filter::recieve_angle(double phi){
    double average_phi = 0;
    current_time_phi = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_phi;
    _debug_print_filter_("phi :" << phi);
    RingBuffer_phi.set(phi);
    average_phi = RingBuffer_phi.get_weighted_average();
    duration_phi = current_time_phi-last_time_phi;
    state[3] = ((average_phi-state[2])/duration_phi.count())*1000;
    state[2] = average_phi;
    publish();
}