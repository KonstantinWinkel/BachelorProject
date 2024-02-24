#include "Pass_filter.h"

void Pass_filter::recieve_p(double p, double u){
    double now = get_time();
    double dt = now-last_p_update;
    last_p_update = now;
    if(!first_p)
        Pass_filter::x_hat[1] = (p - Pass_filter::x_hat[0])/dt;
    else{
        Pass_filter::x_hat[1] = 0;
        first_p = false;
    }
    Pass_filter::x_hat[0] = p;
}

void Pass_filter::recieve_theta(double theta, double u){
    double now = get_time();
    double dt = now-last_theta_update;
    last_theta_update = now;
    if(!first_theta)
        Pass_filter::x_hat[3] = (theta - Pass_filter::x_hat[2])/dt;
    else{
        Pass_filter::x_hat[3] = 0;
        first_theta = false;
    }
    Pass_filter::x_hat[2] = theta;
}