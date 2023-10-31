#ifndef FILTER_H
#define FILTER_H

#include "DataPuffer.h"
#include "FileWriter.h"

#include "bachelor_debug.h"

#if defined(_DEBUG_FILTER_ON_)
#define _debug_print_filter_(x) std::cout << x << std::endl
#else
#define _debug_print_filter_(x)
#endif

#define toradiants 0.01745329251

using namespace je;

class Filter
{
protected:
    Identifier axis;
    std::array<double,4> state;

public:
    ~Filter(){};

    inline double to_m(uint16_t lidar_data){
        return lidar_data/1000.0;
    }

    std::string state_str(std::array<double,4> state){
        std::stringstream s;
        for(int i = 0; i<3;i++) s<< state[i] << ',';
        s<<state[3];
        return s.str();
    }

    virtual void update_pos(uint16_t pos,double last_u, double state[4]) = 0;

    virtual void update_angle(double phi,double last_u, double state[4]) = 0;
};


#endif //FILTER_H