#include "PD_Controller.h"

PD_Controller::PD_Controller(double k[4]){
    for(int i = 0;i<4;i++){
        PD_Controller::k[i] = k[i];
    }
}

void PD_Controller::calculate_u(double x_hat[4], double& u){
    u = 0;
    for(int i = 0;i<4;i++){
        u += -k[i]*(target_state[i]-x_hat[i]);
    }
}