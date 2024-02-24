#ifndef __KF_IMU_H__
#define __KF_IMU_H__

#include <Eigen/Dense>
#include <chrono>
#include <iostream>

//state
#define X_SIZE 14
//IMU: 3 accel, 3 gyro, 3 magneto; LIDAR 2. 3+3+3+2 = 11 (vgl adam ries)
#define Y_SIZE 11
//Input 2 Motos + 3 Bias
#define U_SIZE 5

class Kalman_Filter_IMU{
    protected: 
    public:
    double g = 9.81;
    double dt;
    double l_1;
    double l;
    std::chrono::_V2::system_clock::time_point time_now;
    std::chrono::_V2::system_clock::time_point last_update;
    Eigen::Vector3d north;
    Eigen::Vector<double, U_SIZE> u;
    Eigen::Vector<double, X_SIZE> x_hat_prio; //Schätzung Messwert
    Eigen::Vector<double, X_SIZE> x_hat_pos; //Verbesserte Schätzung. man braucht glaube ich nur einen der beiden Werte aber sicher ist sicher :)
    Eigen::Vector<double, Y_SIZE> z; //Durch Modell vorhergesagte Messung
    Eigen::Vector<double, Y_SIZE> y; //Gemessener Zustand
     Eigen::Matrix<double, Y_SIZE, X_SIZE> H; //Outpit matrix TODO 4*2Matrix
    Eigen::Matrix<double, X_SIZE, X_SIZE> F; //Jakobimartix aus der Zustandsgleichung
    Eigen::Matrix<double, Y_SIZE, Y_SIZE> S; //Kovarianz der Innovation 
    Eigen::Matrix<double, X_SIZE, X_SIZE> P_pri; //Kovarianz der Zustandsvorhersage (unkorrigiert)
    Eigen::Matrix<double, X_SIZE, X_SIZE> P_pos; //Kovarianz der Zustandsvorhersage (korrigiert)
    Eigen::Matrix<double, Y_SIZE, Y_SIZE> R; //Rauschen der Messung
    Eigen::Matrix<double, X_SIZE, X_SIZE> Q; //Rauschen des Modells
    Eigen::Matrix<double, U_SIZE, U_SIZE> variance; 
    Eigen::Matrix<double, X_SIZE, U_SIZE> FU; //Influence of the variances defined in "variance" on Q
    Eigen::Matrix<double, X_SIZE, Y_SIZE> K; //Kalman gain

    bool once = true;

    void virtual update_dt();

    inline void flatten_matrix(Eigen::MatrixXd M, double* d){
        for(int i = 0;i<M.rows();i++)
            for(int j = 0;j<M.cols();j++)
                d[X_SIZE*i+j] = M(i,j);
    }

    Kalman_Filter_IMU(double m,double* Q_diag = 0, double* R_diag = 0);

    void normalize_quaternion();

    void correct_state();

    void update_z();

    void update_Q();

    void update_R();

    void update_F();

    void update_H();

    void predict_state();

    void predict_cycle();

    void filtered_cycle();

    //called to get next measurement
    void next_measurement(Eigen::Vector<double, Y_SIZE> y_k,Eigen::Vector<double, U_SIZE> u, double state[X_SIZE], double dt);

    void next_measurement(Eigen::Vector<double, Y_SIZE> y_k,Eigen::Vector<double, U_SIZE> u, double state[X_SIZE], bool update_dt = true);

    //rest are boring set/get methods ):

    void set_x_hat_prio(double* x_hat_prio){
        for(int i = 0;i < X_SIZE;i++)
            this->x_hat_prio[i] = x_hat_prio[i];
    }

    void get_x_hat_prio(double* x_hat_prio){
        for(int i = 0;i< X_SIZE;i++)
               x_hat_prio[i] = this->x_hat_prio[i];
        
    }

    void set_north(double* north){
        for(int i = 0;i<3;i++)
            this->north[i] = north[i];
    }

    void set_x_hat_pos(double* x_hat_pos){
        for(int i = 0;i < X_SIZE;i++)
            this->x_hat_pos[i] = x_hat_pos[i];
    }

    void get_x_hat_pos(double* x_hat_pos){
        for(int i = 0;i< X_SIZE;i++)
               x_hat_pos[i] = this->x_hat_pos[i];
    }

    void set_dt(double dt){
        this->dt = dt;
    }

    void set_u(double* u){
        for(int i = 0;i<U_SIZE;i++){
            this->u(i,0) = u [i];
        }
    }

    void get_Q(double* Q){
        flatten_matrix(this->Q,Q);
    }

    void get_F(double* F){
        flatten_matrix(this->F,F);
    }

    void get_H(double* H){
        flatten_matrix(this->H,H);
    }

    void get_y(double* y){
        for(int i = 0;i<Y_SIZE;i++){
            y[i] = this->y[i];
        }
    }

    void get_z(double* z){
        for(int i = 0;i<Y_SIZE;i++){
            z[i] = this->z[i];
        }
    }
};

#endif //__KF_H__KF_IMU_H____