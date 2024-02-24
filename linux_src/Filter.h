#ifndef FILTER_H
#define FILTER_H

#include "DataPuffer.h"

#include "bachelor_debug.h"

#if defined(_DEBUG_FILTER_ON_)
#define _debug_print_filter_(x) std::cout << x << std::endl
#else
#define _debug_print_filter_(x)
#endif

class Filter
{
protected:
    double x_hat[4] = {0,0,0,0};
    std::chrono::_V2::system_clock::time_point beginning_timestamp = std::chrono::high_resolution_clock::now();

    virtual double get_time(){
        return (beginning_timestamp-std::chrono::high_resolution_clock::now()).count()*1000;
    }

public:
    std::string state_str(double x_hat[4]){
        std::stringstream s;
        s<<x_hat[0];
        for(int i = 1; i<4;i++) s << ',' << x_hat[i];
        return s.str();
    }

    void get_state(double x_hat[4]){
        for(int i = 0;i<4;i++){
            x_hat[i] = this->x_hat[i];
        }
    }

    virtual void recieve_p(double p, double last_u) = 0;

    virtual void recieve_theta(double theta, double last_u) = 0;

    //virtual void recieve_y(double p, double theta, double u) = 0;
};


#endif //FILTER_H