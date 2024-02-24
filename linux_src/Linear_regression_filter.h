#ifndef LINEAR_REGRESSION_FILTER_H
#define LINEAR_REGRESSION_FILTER_H

#include "DataPuffer.h"
#include "Filter.h"
#include <chrono>
#include <fstream>

class Linear_regression_filter : public Filter{

private:
    double camera_latency;
    double lidar_latency;
    linear_regression lr_x;
    linear_regression lr_theta;

public:

    Linear_regression_filter(size_t size_x = 10, size_t size_theta = 10, double camera_latency = 0.133, double lidar_latency = 0);

    void recieve_p(double p, double u);

    void recieve_theta(double theta, double u);
};

#endif //LINEAR_REGRESSION_FILTER_H