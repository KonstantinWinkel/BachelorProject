#include "EKF.h"

Extended_Kalman_Filter::Extended_Kalman_Filter()
{
    double Q[4] = {1e-4, 1e-4, 1e-7, 1e-7}; // process noise (gets added to state)
    double R[2] = {5e-4, 5e-2}; // measurement noise (gets added to sensor measurements)

    Extended_Kalman_Filter::H << 1,0,0,0,
                        0,0,1,0;
                        
    Extended_Kalman_Filter::Q << 0.2, 0.3, 0, 0 ,
                        0.3, 0.1, 0, 0,
                        0, 0, 0.1, 0.3, 
                        0, 0, 0.3, 0.1;

    Extended_Kalman_Filter::R << 1,0,
                        0,0.5;
    
    ny_u << 0.001, 0, 0,
            0, 150, 0,
            0, 0, 200;
    x_hat_pos << 0,0,0,0;
    x_hat = &x_hat_pos[0];
}

void Extended_Kalman_Filter::predict(){
    
    double now = get_time();
    dt = now-last_update;
    last_update = now;

    // Zustandsvorhersage
    x_hat_prio(0) = x_hat_pos(0) + dt*x_hat_pos(1) + 0.5*dt*dt*u;
    x_hat_prio(1) = x_hat_pos(1) + dt*u;
    double d_omega = 1.5*(g*sin(x_hat_pos(2))+u*cos(x_hat_pos(2)))/L;
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

void Extended_Kalman_Filter::update_Q(){
    double hdt2 = 0.5 * dt * dt;
    double cos_theta_inv_L = cos(x_hat_prio[2])/L;
    Fu <<   hdt2, dt, 0,
            dt, 1, 0,
            hdt2*cos_theta_inv_L, 0, dt,
            dt * cos_theta_inv_L, 0, 1;
    //std::cout << "FU:" << Fu << ", theta:" << x_hat_prio[2] << ", dt:" << dt << std::endl ;
    Q = Fu*(ny_u*ny_u)*Fu.transpose();
}

void Extended_Kalman_Filter::update_F(){
    Extended_Kalman_Filter::F << 0,dt,0,0,
                        0,0,0,0,
                        0,0,0,dt,
                        0,0,1.5*dt/L*(u*sin(x_hat_pos(2)-g*cos(x_hat_pos(2)))),0;
}

void Extended_Kalman_Filter::update(){
    //update F and H
    update_F();

    //compute new process noise matrix
    //update_Q();

    //Kovarianz predict
    P_pri = F * P_pos * F.transpose() + Q;

    //Kovarianz Innovation
    S = H*P_pri*H.transpose()+R;

    //update K
    K = P_pri*H.transpose()*S.inverse();

    //update Position (will be published by the recieve_ method)
    P_pos = (Eigen::Matrix4d::Identity()-K*H)*P_pri;
}

void Extended_Kalman_Filter::recieve_pos(double pos, double u){
    y(0) = pos;
    Extended_Kalman_Filter::u = u;
    Extended_Kalman_Filter::predict();
    Extended_Kalman_Filter::update();
}

void Extended_Kalman_Filter::recieve_angle(double angle, double u){
    y(1) = angle;
    Extended_Kalman_Filter::u = u;
    Extended_Kalman_Filter::predict();
    Extended_Kalman_Filter::update();
}

void Extended_Kalman_Filter::recieve_y(double p, double theta, double u){
    y(0) = p;
    y(1) = theta;
    Extended_Kalman_Filter::u = u;
    Extended_Kalman_Filter::predict();
    Extended_Kalman_Filter::update();
}