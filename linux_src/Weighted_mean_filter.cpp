#include "Weighted_mean_filter.h"
#include <iostream>
#include "thread"

#include "bachelor_debug.h"

#if defined(_DEBUG_FILTER_ON_)
#define _debug_print_filter_(x) std::cout << x << std::endl
#else
#define _debug_print_filter_(x)
#endif

Weighted_mean_filter::Weighted_mean_filter(size_t size_p, size_t size_theta, const std::function<double(int)> lambda_p, const std::function<double(int)> lambda_theta)
    :RingBuffer_p(size_p,lambda_p),RingBuffer_theta(size_theta,lambda_theta){
    last_p_update = get_time();
    last_theta_update = get_time();
}

void Weighted_mean_filter::recieve_p(double p, double u){
    //update time
    double now = get_time();
    double dt = now-last_p_update;
    last_p_update = now;

    //write new value into Buffer
    RingBuffer_p.set(p);
    double p_k = RingBuffer_p.get_weighted_average();

    //X_hat[0] = p_k-1 in this case. if case to ensure v is not set to nonsens after the first measurement
    if(!first_p) Weighted_mean_filter::x_hat[1] = (p_k-Weighted_mean_filter::x_hat[0])/dt;
    else {
        Weighted_mean_filter::x_hat[1] = 0;
        first_p = false;
    }
    Weighted_mean_filter::x_hat[0] = p_k;
}

void Weighted_mean_filter::recieve_theta(double theta, double u){
    //update time
    double now = get_time();
    double dt = now-last_theta_update;
    last_theta_update = now;

    //write new measurement into Buffer
    RingBuffer_theta.set(theta);
    double theta_k = RingBuffer_theta.get_weighted_average();

    //ensure that omega is not set to nonsene after the first measurement + 
    if(!first_theta) 
        Weighted_mean_filter::x_hat[3] = (theta_k-Weighted_mean_filter::x_hat[2])/dt;
    else {
        Weighted_mean_filter::x_hat[3] = 0;
        first_theta = false;
    }
    Weighted_mean_filter::x_hat[2] = theta_k;
}