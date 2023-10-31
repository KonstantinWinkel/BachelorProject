#include "KF.h"

#define TESTWERT 0.3
//#define TESTWERT 0

#define OFFSET 0

Kalman_Filter::Kalman_Filter(double latency){
    Kalman_Filter::H << 1,0,0,0,
                        0,0,1,0;
    Kalman_Filter::Q << 0.2, TESTWERT, 0, 0 ,
                        TESTWERT, 0.1, 0, 0,
                        0, 0, 0.1, TESTWERT, 
                        0, 0, TESTWERT, 0.1; 
    Kalman_Filter::R << 1,0,
                        0,0.5;
    
    ny_u << 0, 0, 0,
            0, 200, 0,
            0, 0, 200;
    x_hat_pos << 0.01,OFFSET,0.1,OFFSET;
}

void Kalman_Filter::update_dt(){
    time_now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = last_update - time_now;
    dt = ms_double.count()/1000;
}

void Kalman_Filter::predict(){
    update_dt();
    // Zustandsvorhersage
    x_hat_prio(0) = x_hat_pos(0) + dt*x_hat_pos(1) + 0.5*dt*dt*u;
    x_hat_prio(1) = x_hat_pos(1) + dt*u;
    double d_omega = (g*sin(x_hat_pos(2))+u*cos(x_hat_pos(2)))/L;
    x_hat_prio(2) = x_hat_pos(2) + dt*x_hat_pos(3) + 0.5*dt*dt*d_omega;
    x_hat_prio(3) = x_hat_pos(3) + dt*d_omega;

    // Messvorhersage (nur pos und angle werden gemessen)
    z(0) = x_hat_prio(0);
    z(1) = x_hat_prio(2);

    //Berechnung der Innovation
    nu = y - z;

    //Berechnung der neuen Schätzung
    x_hat_pos = x_hat_prio + K*nu;
}

void Kalman_Filter::update_Q(){
    double hdt2 = 0.5 * dt * dt;
    double cos_theta_inv_L = cos(x_hat_prio[2])/L;
    Fu <<   hdt2, dt, 0,
            dt, 1, 0,
            hdt2*cos_theta_inv_L, 0, dt,
            dt * cos_theta_inv_L, 0, 1;
    //std::cout << "FU:" << Fu << ", theta:" << x_hat_prio[2] << ", dt:" << dt << std::endl ;
    Q = Fu*(ny_u*ny_u)*Fu.transpose();
}

void Kalman_Filter::update_F(){
    Kalman_Filter::F << 0,dt,0,0,
                        0,0,0,0,
                        0,0,0,dt,
                        0,0,dt/L*(u*sin(x_hat_prio(2)-g*cos(x_hat_prio(2)))),0;
}

void Kalman_Filter::update(){
    //update F and H
    update_F();

    //compute new process noise matrix
    update_Q();

    //std::cout << "ny_u:" << ny_u << ", Q:\n" << Q << std::endl; 
    //std::cout << Q << std::endl; 

    //Kovarianz predict
    P_pri = F * P_pos * F.transpose() + Q;

    //Kovarianz Innovation
    S = H*P_pri*H.transpose()+R;

    //update K
    K = P_pri*H.transpose()*S.inverse();

    //update Position (will be published by the recieve_ method)
    P_pos = (Eigen::Matrix4d::Identity()-K*H)*P_pri;
}

void Kalman_Filter::next_measurement(double y0, double y1, double u, double state[4]){ //used for the simulation.
    y(0) = y0;
    y(1) = y1; 
    Kalman_Filter::u = u;
    Kalman_Filter::update();
    Kalman_Filter::predict();
    for(int i = 0;i<4;i++) state[i] = x_hat_pos(i);
}

void Kalman_Filter::update_angle(double angle, double u, double state[4]){
    y(1) = angle;
    Kalman_Filter::u = u;
    Kalman_Filter::update();
    Kalman_Filter::predict();
    for(int i = 0;i<4;++i) Filter::state[i] = x_hat_pos(i);
}

void Kalman_Filter::update_pos(uint16_t pos, double u, double state[4]){
    y(0) = to_m(pos);
    Kalman_Filter::u = u;
    Kalman_Filter::update();
    Kalman_Filter::predict();
    for(int i = 0;i<4;i++) state[i] = x_hat_pos(i);
}