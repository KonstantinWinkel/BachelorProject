#ifndef LINEAR_REGRESSION_FILTER_H
#define LINEAR_REGRESSION_FILTER_H

#include "DataPuffer.h"
#include "Filter.h"

class Linear_regression_filter : public Filter{

private:
    double camera_latency;
    double lidar_latency;
    double time_now;
    linear_regression lr_x;
    linear_regression lr_phi;
    std::chrono::_V2::system_clock::time_point start_time;
    std::string path;
    std::fstream file;

double time_since_start(std::chrono::_V2::system_clock::time_point time);

public:

Linear_regression_filter(Controller * controller, double camera_latency = 0.133, double lidar_latency = 0.1, size_t size_x = 10, size_t size_phi = 10);

void write_to_file(double new_value, std::array<double,4> state);

void update_angle();

void update_pos();

void recieve_pos(uint16_t pos_uint);

void recieve_angle(double phi);
};

#endif //LINEAR_REGRESSION_FILTER_H