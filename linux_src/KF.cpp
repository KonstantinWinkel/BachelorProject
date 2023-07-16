#include "KF.h"

Kalman_Filter::Kalman_Filter(double latency){
    Kalman_Filter::H << 1,0,0,0,
                        0,1,0,0; //sehr unsicher ob das Sinn macht ):
    Kalman_Filter::Q << 100, 0, 0, 0,
                        0, 10, 0, 0,
                        0, 0, 1, 0, 
                        0, 0, 0, 0.1; 
    Kalman_Filter::R << 100,0,
                        0,0.1;
    

}

void Kalman_Filter::update_dt(){
    dt = 0.001;
    /*time_now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = last_update - time_now;
    dt = ms_double.count()/1000;*/
}

void Kalman_Filter::predict(){
    // Zustandsvorhersage
    x_hat_prio(0) = x_hat_pos(0) + dt*x_hat_pos(1);
    x_hat_prio(1) = x_hat_pos(1) + dt*u;
    x_hat_prio(2) = x_hat_pos(2) + dt*x_hat_pos(3);
    x_hat_prio(3) = x_hat_pos(3) + dt*(g*sin(x_hat_pos(2))+u*cos(x_hat_pos(2)))/L;

    // Messvorhersage (nur pos und angle werden gemessen)
    z(0) = x_hat_prio(0);
    z(1) = x_hat_prio(2);

    //Berechnung der Innovation
    nu = y - z;

    //Berechnung der neuen Schätzung
    x_hat_pos = x_hat_prio + K*nu;
}

void Kalman_Filter::update_F(){
    Kalman_Filter::F << 0,1,0,0,
                        0,0,1,0,
                        0,0,0,1,
                        0,0,1/L*(g*cos(x_hat_prio(2)-u*cos(x_hat_prio(2)))),0;
}

void Kalman_Filter::update(){
    //update F and H
    update_F();
    //update_H(); no need... H does not depend on anything. This version is needed for the generic version which is why it stays as a comment.

    //Kovarianz predict
    P_pri = F * P_pos * F.transpose() + Q;

    //Kovarianz Innovation
    S = H*P_pri*H.transpose()+R;

    //update K
    K = P_pri*H.transpose()*S.inverse();

    //update Position (will be published by the recieve_ method)
    P_pos = (Eigen::Matrix4d::Identity()-K*H)*P_pri;
}

void Kalman_Filter::next_measurement(double y0, double y1, double state[4]){ //used for the simulation.
    y(0) = y0;
    y(1) = y1; 
    Kalman_Filter::predict();
    Kalman_Filter::update();
    for(int i = 0;i<4;i++) 
    state[i] = x_hat_pos(i);
}

void Kalman_Filter::update_angle(double angle, double u, double state[4]){
    y(1) = angle;
    Kalman_Filter::predict();
    Kalman_Filter::update();
    for(int i = 0;i<4;++i) Filter::state[i] = state[i];
}

void Kalman_Filter::update_pos(uint16_t pos, double u, double state[4]){
    y(0) = to_m(pos);
    Kalman_Filter::predict();
    Kalman_Filter::update();
    for(int i = 0;i<4;++i) Filter::state[i] = state[i];
}