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
protected:
    std::array<double,4> state;
    Controller * controller;

public:
    ~Filter(){};

    virtual void publish(){
        //_debug_print_filter_("published state :" << state_str(state));
        controller->recieve_data(state);
    }

    virtual void recieve_pos(uint16_t pos) = 0;

    virtual void recieve_angle(double phi) = 0;
};


#endif //FILTER_H