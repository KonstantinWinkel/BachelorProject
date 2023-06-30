#ifndef FILTER_H
#define FILTER_H

#include "DataPuffer.h"
#include "Controller.h"

#include "bachelor_debug.h"

#if defined(_DEBUG_FILTER_ON_)
#define _debug_print_filter_(x) std::cout << x << std::endl
#else
#define _debug_print_filter_(x)
#endif

using namespace je;

class Filter
{
private:
    double x,y,phi,omega;
    uint16_t bias;
    std::array<double,4> state;
    RingBuffer<double> RingBuffer_x;
    RingBuffer<double> RingBuffer_phi;

    uint16_t pos_uint;
    std::chrono::_V2::system_clock::time_point current_time_x;
    std::chrono::_V2::system_clock::time_point last_time_x;
    std::chrono::_V2::system_clock::time_point current_time_phi;
    std::chrono::_V2::system_clock::time_point last_time_phi;

    Controller * controller;
    
public:
    Filter(Controller * controller, size_t size_x = 10, size_t size_phi = 5);
    ~Filter();

    virtual void publish();

    virtual double to_m(uint16_t lidar_data);

    virtual void recieve_pos(uint16_t pos);

    virtual void recieve_angle(double phi);
};


#endif //FILTER_H