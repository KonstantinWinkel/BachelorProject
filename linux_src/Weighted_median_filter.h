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

Weighted_median_filter(Controller * controller,size_t size_x = 10, size_t size_phi = 3);

inline double to_radiants(double angle);

double to_m(uint16_t lidar_data);

std::string state_str(std::array<double,4> state);

void recieve_pos(uint16_t pos_uint);

void recieve_angle(double phi);
};

#endif //WEIGHTED_MEDIAN_FILTER_H