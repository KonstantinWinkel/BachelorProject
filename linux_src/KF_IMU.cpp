#include "KF_IMU.h"
#include <iostream>
#define dp(x) std::cout << #x<<':' << x << std::endl

#define sqr(x) (x)*(x)

Kalman_Filter_IMU::Kalman_Filter_IMU(double l,double* Q_diag, double* R_diag){
    this->P_pos = 10*Eigen::Matrix<double, X_SIZE, X_SIZE>::Identity();
    this->l = l;
    this->l_1 = 1/l;
    this->variance = Eigen::DiagonalMatrix<double, U_SIZE>(2.43531,5.50064,-0.061858,9.1547,0.983008);
    north << 5,2,1;
    //q,w,x,pos,v
    this->x_hat_pos<<1,0,0,0 ,0,0,0 ,0,0,0 ,0,0 ,0,0;
    if(Q_diag != 0)
    this->Q = 100*Eigen::DiagonalMatrix<double, X_SIZE>(Q_diag[0],Q_diag[1],Q_diag[2],Q_diag[3],Q_diag[4],Q_diag[5],Q_diag[6],Q_diag[7],Q_diag[8],Q_diag[9],Q_diag[10],Q_diag[11],Q_diag[12],Q_diag[13]);
    if(Q_diag != 0)
    this->R = Eigen::DiagonalMatrix<double, Y_SIZE>(R_diag[0],R_diag[1],R_diag[2],R_diag[3],R_diag[4],R_diag[5],R_diag[6],R_diag[7],R_diag[8],R_diag[9],R_diag[10]);
}

//normalizes x_hat_pos[0-3]
void Kalman_Filter_IMU::normalize_quaternion(){
    //calculate square sum
    double square_sum = 0;
    for(int i = 0; i<4;i++){
        square_sum += x_hat_pos[i]*x_hat_pos[i];
    }
    //calculate factor used to normalize the quaternion
    double nom = 1/sqrt(square_sum);

    //carry out normalization
    for(int i = 0; i<4;i++){
        x_hat_pos[i] = x_hat_pos[i]*nom;
    }
}

void Kalman_Filter_IMU::update_dt(){
    time_now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = last_update - time_now;
    dt = ms_double.count()/1000;
}

void Kalman_Filter_IMU::update_Q(){
    this->FU<<  0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, -((dt*(sqr(x_hat_pos[0]) - sqr(x_hat_pos[1]) + sqr(x_hat_pos[2]) - sqr(x_hat_pos[3])))*l_1), 0, 0, 0,
                ((2*dt*(x_hat_pos[0]*x_hat_pos[1] - x_hat_pos[2]*x_hat_pos[3]))*l_1), ((2*dt*(x_hat_pos[1]*x_hat_pos[2] + x_hat_pos[0]*x_hat_pos[3]))*l_1), 0, 0, 0,
                ((dt*(sqr(x_hat_pos[0]) - sqr(x_hat_pos[1]) + sqr(x_hat_pos[2]) - sqr(x_hat_pos[3])))*l_1), 0, 0, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 0, 1, 0,
                0, 0, 0, 0, 1,
                (sqr(dt)), 0, 0, 0, 0,
                0, sqr(dt), 0, 0, 0,
                dt, 0, 0, 0, 0,
                0, dt, 0, 0, 0;
    this->Q = FU*(variance*variance)*(FU.transpose());
}

void Kalman_Filter_IMU::update_F(){
    double dt_2 = 0.5*dt;
    this->F <<  1, -dt_2*x_hat_pos[4], -dt_2*x_hat_pos[5], -dt_2*x_hat_pos[6], -dt_2*x_hat_pos[1], -dt_2*x_hat_pos[2], -dt_2*x_hat_pos[3], 0, 0, 0, 0, 0, 0, 0,
                 dt_2*x_hat_pos[4], 1,  dt_2*x_hat_pos[6], -dt_2*x_hat_pos[5],  dt_2*x_hat_pos[0], -dt_2*x_hat_pos[3],  dt_2*x_hat_pos[2], 0, 0, 0, 0, 0, 0, 0,
                 dt_2*x_hat_pos[5], -dt_2*x_hat_pos[6], 1,  dt_2*x_hat_pos[4],  dt_2*x_hat_pos[3],  dt_2*x_hat_pos[0], -dt_2*x_hat_pos[1], 0, 0, 0, 0, 0, 0, 0,
                 dt_2*x_hat_pos[6],  dt_2*x_hat_pos[5], -dt_2*x_hat_pos[4], 1, -dt_2*x_hat_pos[2],  dt_2*x_hat_pos[1],  dt_2*x_hat_pos[0], 0, 0, 0, 0, 0, 0, 0,
                 2*dt*(g *x_hat_pos[1]-x_hat_pos[0]*u[1])*l_1, 2*dt*(g *x_hat_pos[0]+x_hat_pos[1]*u[1])*l_1, -2*dt*(g *x_hat_pos[3]+x_hat_pos[2]*u[1])*l_1,-2*dt*(g *x_hat_pos[2] - x_hat_pos[3]*u[1])*l_1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 2*dt*(x_hat_pos[1]*u[0]+x_hat_pos[3]*u[1])*l_1, 2*dt*(x_hat_pos[0]*u[0]+x_hat_pos[2]*u[1])*l_1, -2*dt*(x_hat_pos[3]*u[0]-x_hat_pos[1]*u[1])*l_1,-2*dt*(x_hat_pos[2]*u[0] - x_hat_pos[0]*u[1])*l_1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                 2*dt*(g *x_hat_pos[3]+x_hat_pos[0]*u[0])*l_1, 2*dt*(g *x_hat_pos[2]-x_hat_pos[1]*u[0])*l_1,  2*dt*(g *x_hat_pos[1]+x_hat_pos[2]*u[0])*l_1, 2*dt*(g *x_hat_pos[0] - x_hat_pos[3]*u[0])*l_1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, dt, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, dt,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1;
}

void Kalman_Filter_IMU::update_H(){
    this->H <<  0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0,
            2*(north[0]*x_hat_pos[0] + north[2]*-x_hat_pos[2] + north[1]*x_hat_pos[3]),  2*(north[0]*x_hat_pos[1] + north[1]*x_hat_pos[2] + north[2]*x_hat_pos[3]), -2*(north[2]*x_hat_pos[0] - north[1]* x_hat_pos[1] + north[0]*x_hat_pos[2]),  2*(north[1]*x_hat_pos[0] + north[2]*x_hat_pos[1] -north[0]*x_hat_pos[3]), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            2*(north[1]*x_hat_pos[0] + north[2]* x_hat_pos[1] - north[0]*x_hat_pos[3]),  2*(north[2]*x_hat_pos[0] - north[1]*x_hat_pos[1] + north[0]*x_hat_pos[2]),  2*(north[0]*x_hat_pos[1] + north[1]* x_hat_pos[2] + north[2]*x_hat_pos[3]), -2*(north[0]*x_hat_pos[0] - north[2]*x_hat_pos[2] +north[1]*x_hat_pos[3]), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            2*(north[2]*x_hat_pos[0] - north[1]* x_hat_pos[1] + north[0]*x_hat_pos[2]), -2*(north[1]*x_hat_pos[0] + north[2]*x_hat_pos[1] - north[0]*x_hat_pos[3]),  2*(north[0]*x_hat_pos[0] + north[2]*-x_hat_pos[2] + north[1]*x_hat_pos[3]),  2*(north[0]*x_hat_pos[1] + north[1]*x_hat_pos[2] +north[2]*x_hat_pos[3]), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            -2*(g*x_hat_pos[3] - x_hat_pos[0]*u[0] + x_hat_pos[2]*u[1]), -2*g*x_hat_pos[2] + 2*x_hat_pos[1]*u[0] + 2*x_hat_pos[3]*u[1], -2*(g*x_hat_pos[1] + x_hat_pos[2]*u[0] + x_hat_pos[0]*u[1]), -2*(g*x_hat_pos[0] + x_hat_pos[3]*u[0] - x_hat_pos[1]*u[1]), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            -2*(g*x_hat_pos[0] + x_hat_pos[3]*u[0] - x_hat_pos[1]*u[1]), 2*(g*x_hat_pos[1] + x_hat_pos[2]*u[0] + x_hat_pos[0]*u[1]), -2*g*x_hat_pos[2] + 2*x_hat_pos[1]*u[0] + 2*x_hat_pos[3]*u[1], 2*(g*x_hat_pos[3] - x_hat_pos[0]*u[0] + x_hat_pos[2]*u[1]), 2*l*x_hat_pos[4], 2*l*x_hat_pos[5], 2*l*x_hat_pos[6], 0, 0, 0, 0, 0, 0, 0,
            2*(g*x_hat_pos[1] + x_hat_pos[2]*u[0] + x_hat_pos[0]*u[1]), 2*(g*x_hat_pos[0] + x_hat_pos[3]*u[0] - x_hat_pos[1]*u[1]), -2*(g*x_hat_pos[3] - x_hat_pos[0]*u[0] + x_hat_pos[2]*u[1]), -2*g*x_hat_pos[2] + 2*x_hat_pos[1]*u[0] + 2*x_hat_pos[3]*u[1], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0;
}   

void Kalman_Filter_IMU::update_z(){
    //POV: IMU -> Cojugated
    Eigen::Quaternion<double> q_k(x_hat_prio[0],-x_hat_prio[1],-x_hat_prio[2],-x_hat_prio[3]);
    Eigen::Matrix3d R_BN = q_k.toRotationMatrix();
    Eigen::Vector3d north_vec = R_BN*Eigen::Vector3d(north[0],north[1],north[2]);
    Eigen::Vector3d def_acc = R_BN*Eigen::Vector3d(u[0],-g,u[1]);
    //IMU
    //gyro
    z[0] = x_hat_prio[4] + x_hat_prio[7];
    z[1] = x_hat_prio[5] + x_hat_prio[8];
    z[2] = x_hat_prio[6] + x_hat_prio[9];
    //magnetometer
    z[3] = north_vec[0];
    z[4] = north_vec[1];
    z[5] = north_vec[2];
    //accell
    z[6] = def_acc[0];
    z[7] = def_acc[1]+(sqr(x_hat_prio[4])+sqr(x_hat_prio[5])+sqr(x_hat_prio[6]))/l_1;
    z[8] = def_acc[2];
    //LIDAR
    z[9] = x_hat_prio[10];
    z[10] = x_hat_prio[11];
}

void Kalman_Filter_IMU::predict_state(){ //applies f(state,u)
    //update_dt();

    double nom = 1/sqrt(x_hat_pos[0]*x_hat_pos[0]+x_hat_pos[1]*x_hat_pos[1]+x_hat_pos[2]*x_hat_pos[2]+x_hat_pos[3]*x_hat_pos[3]);
            for(int i = 0;i<4;i++) x_hat_pos[i] = x_hat_pos[i] * nom;

    //compute some stuff to let Eigen do all the work (and save the old state)
    Eigen::Quaternion<double> q_k(x_hat_pos[0],x_hat_pos[1],x_hat_pos[2],x_hat_pos[3]);

    //angular velocity in Body Frame
    Eigen::Vector3d omega(x_hat_pos[4],x_hat_pos[5],x_hat_pos[6]);

    //if w is in body Frame
    Eigen::Quaternion<double> d_q= q_k*Eigen::Quaternion<double>(0,0.5*omega[0],0.5*omega[1],0.5*omega[2]);

    //position of the pencil devided by its length
    //this is just the second column of R_NB why did I write this /:
    double r_l[] = {
        -2*(x_hat_pos[1]*x_hat_pos[2]+x_hat_pos[0]*x_hat_pos[3]),
        -1*(sqr(x_hat_pos[0])-sqr(x_hat_pos[1])+sqr(x_hat_pos[2])-sqr(x_hat_pos[3])),
        -2*(x_hat_pos[2]*x_hat_pos[3]-x_hat_pos[0]*x_hat_pos[1])
    };
    x_hat_prio[0] = x_hat_pos[0]  + dt * (d_q.w());
    x_hat_prio[1] = x_hat_pos[1]  + dt * (d_q.x());
    x_hat_prio[2] = x_hat_pos[2]  + dt * (d_q.y());
    x_hat_prio[3] = x_hat_pos[3]  + dt * (d_q.z());
    x_hat_prio[4] = x_hat_pos[4]  + dt * ((l_1 * (g*r_l[2]+r_l[1]*u[1]))   );
    x_hat_prio[5] = x_hat_pos[5]  + dt * ((l_1 * (r_l[2]*u[0]-r_l[0]*u[1])));
    x_hat_prio[6] = x_hat_pos[6]  + dt * ((l_1 * (-g*r_l[0]-r_l[1]*u[0]))  );
    x_hat_prio[7] = x_hat_pos[7];
    x_hat_prio[8] = x_hat_pos[8];
    x_hat_prio[9] = x_hat_pos[9];
    x_hat_prio[10]= x_hat_pos[10] + dt*(x_hat_pos[12]+dt*u[0]);
    x_hat_prio[11]= x_hat_pos[11] + dt*(x_hat_pos[13]+dt*u[1]);
    x_hat_prio[12]= x_hat_pos[12] + dt*(u[0]);
    x_hat_prio[13]= x_hat_pos[13] + dt*(u[1]);
}

void Kalman_Filter_IMU::predict_cycle(){

    //predict next state
    predict_state();

    /*std::cout << x_hat_prio[0];
    for(int i = 1;i<X_SIZE;i++){
        std::cout << ',' << x_hat_prio[i];
    }
    std::cout << '\n';*/
    update_F();

    update_Q();
    //predict state Kovariance
    P_pri = F * P_pos * F.transpose() + Q;
}

void Kalman_Filter_IMU::filtered_cycle(){

    //calculates H_k+1 and F_k+1
    update_H();
    //update_F();

    //calculate K_K+1
    S = H*P_pri*H.transpose()+R;
    K = P_pri*H.transpose()*S.inverse();

    //calculate z_K+1
    update_z();

    //calculate x_hat_k+1_k+1
    x_hat_pos = x_hat_prio + K*(y-z);

    //calculate P_k+1_k+1
    P_pos = (Eigen::Matrix<double, X_SIZE, X_SIZE>::Identity()-K*H)*P_pri;
}

//Weird structure because I wanted to be able to set dt for the simulation.
void Kalman_Filter_IMU::next_measurement(Eigen::Vector<double, Y_SIZE> y,Eigen::Vector<double, U_SIZE> u, double estimated_state[X_SIZE], double dt){
    Kalman_Filter_IMU::dt = dt;
    next_measurement(y,u,estimated_state,false);
}

void Kalman_Filter_IMU::next_measurement(Eigen::Vector<double, Y_SIZE> y,Eigen::Vector<double, U_SIZE> u, double estimated_state[X_SIZE], bool update_dt){
    if(update_dt)
        Kalman_Filter_IMU::update_dt();
    Kalman_Filter_IMU::predict_cycle();
    Kalman_Filter_IMU::y = y;
    Kalman_Filter_IMU::u = u;
    Kalman_Filter_IMU::filtered_cycle();
    Kalman_Filter_IMU::normalize_quaternion();
    for(int i = 0;i<X_SIZE;i++) estimated_state[i] = x_hat_pos(i);
    once = false;
}