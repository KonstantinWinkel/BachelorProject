#include "Filter.h"

Filter::Filter(CommBuffer<std::array<double,4>> * CommBuffer_filtered_data, CommBuffer<double> * CommBuffer_x, CommBuffer<double> * CommBuffer_y, CommBuffer<double> * CommBuffer_phi, CommBuffer<double> * CommBuffer_omega){
    Filter::CommBuffer_filtered_data = CommBuffer_filtered_data;
    Filter::CommBuffer_x = CommBuffer_x;
    Filter::CommBuffer_y = CommBuffer_y;
    Filter::CommBuffer_phi = CommBuffer_phi;
    Filter::CommBuffer_omega = CommBuffer_omega;
}