#ifndef PASS_FILTER_H
#define PASS_FILTER_H

#include "Filter.h"

class Pass_filter : public Filter{

    double last_p_update;
    double last_theta_update;
    bool first_p = true;
    bool first_theta = true;

    public:
    void recieve_p(double p, double u);
    void recieve_theta(double theta, double u);
};

#endif