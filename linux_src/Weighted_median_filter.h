#ifndef WEIGHTED_MEDIAN_FILTER_H
#define WEIGHTED_MEDIAN_FILTER_H

#include "DataPuffer.h"
#include "Filter.h"
#include <chrono>

class Weighted_median_filter : public Filter{

private:
    double sum;
    int no;
    double x,y,phi,omega;
    uint16_t bias;
    RingBuffer<double> RingBuffer_x;
    RingBuffer<double> RingBuffer_phi;

    std::chrono::_V2::system_clock::time_point current_time_x;
    std::chrono::_V2::system_clock::time_point last_time_x;
    std::chrono::_V2::system_clock::time_point current_time_phi;
    std::chrono::_V2::system_clock::time_point last_time_phi;

public:

Weighted_median_filter(Identifier axis,size_t size_x = 10, size_t size_phi = 3);

void update_pos(uint16_t pos_uint, double last_u, double state[4]);

void update_angle(double phi, double last_u, double state[4]);
};

#endif //WEIGHTED_MEDIAN_FILTER_H