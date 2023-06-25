#ifndef FILTER_H
#define FILTER_H

#include "DataPuffer.h"

using namespace je;

class Filter
{
private:
    double x,y,phi,omega;
    CommBuffer<std::array<double,4>> * CommBuffer_filtered_data;
    CommBuffer<double> * CommBuffer_x;
    CommBuffer<double> * CommBuffer_y;
    CommBuffer<double> * CommBuffer_phi;
    CommBuffer<double> * CommBuffer_omega;
    
public:
    Filter(CommBuffer<std::array<double,4>> * CommBuffer_filtered_data, CommBuffer<double> * CommBuffer_x, CommBuffer<double> * CommBuffer_y, CommBuffer<double> * CommBuffer_phi, CommBuffer<double> * CommBuffer_omega);
    ~Filter();
};


#endif //FILTER_H