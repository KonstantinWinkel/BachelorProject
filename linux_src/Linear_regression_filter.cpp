#include "Linear_regression_filter.h"
#include <chrono>
#include <iomanip>

Linear_regression_filter::Linear_regression_filter(size_t size_x, size_t size_theta, double camera_latency, double lidar_latencysize_t)
    :lr_x(size_x),lr_theta(size_theta){

    Linear_regression_filter::camera_latency = camera_latency;
    Linear_regression_filter::lidar_latency = lidar_latency;
}

void Linear_regression_filter::recieve_p(double p, double last_u){
    double time_now = get_time();
    lr_x.set(datapoint{time_now,p});

    //calculate p,v
    x_hat[1] = lr_x.get_slope();
    x_hat[0] = lr_x.get_intercept() + (time_now+lidar_latency)*x_hat[1];
}

void Linear_regression_filter::recieve_theta(double theta, double last_u){
    //creat new datapoint
    double time_now = get_time();
    lr_theta.set(datapoint{time_now,theta});

    //calculate theta,omega
    x_hat[3] = lr_theta.get_slope();
    x_hat[2] = lr_theta.get_intercept() + (time_now+camera_latency)*x_hat[3];
}