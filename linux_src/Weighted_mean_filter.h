#ifndef WEIGHTED_MEAN_FILTER_H
#define WEIGHTED_MEAN_FILTER_H

#include "DataPuffer.h"
#include "Filter.h"
#include <chrono>

class Weighted_mean_filter : public Filter{

private:

    //not sure why this is here
    double sum;

    //timestamp of the last time a measurement has been recieved
    double last_p_update;
    double last_theta_update;

    //ensures only valid values for v and omega
    bool first_p = true;
    bool first_theta = true;

    //calculates convolution of the sequence of measurements and discrete funktion passed in the constructor
    RingBuffer<double> RingBuffer_p;
    RingBuffer<double> RingBuffer_theta;



public:

    Weighted_mean_filter(size_t size_p = 10, size_t size_theta = 3, const std::function<double(int)> lambda_p = [](int index){return 1;}, const std::function<double(int)> lambda_theta = [](int index){return 1;});

    void recieve_p(double pos, double u);

    void recieve_theta(double theta, double u);
};

#endif //WEIGHTED_MEAN_FILTER_H