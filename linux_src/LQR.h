#ifndef __LQR_H__
#define __LQR_H__
//../Github/BachelorProject/linux_src/
#include "Controller.h"
#include <Eigen/Dense>

//Linear quadratic regulator 

class LQR : public Controller{
    /*
    Eigen::Matrix<double,4,4> A;
    Eigen::Matrix<double,4,4> Q;
    Eigen::Matrix<double,4,4> fancy_A;
    Eigen::Matrix<double,4,4> fancy_Q;
    Eigen::Matrix<double,1,1> R;
    Eigen::Matrix<double,4,1> K;
    Eigen::Matrix<double,4,1> N;
    Eigen::Matrix<double,4,1> B;
    Eigen::Matrix<double,4,1> state_vct;
    Eigen::Matrix<double,4,1> desired_state;
    */ //Eigen website down can't be bothered to find it on other sites. For now it stays here

    Eigen::Matrix4d A;
    Eigen::Matrix4d Q;
    Eigen::Matrix4d fancy_A;
    Eigen::Matrix4d fancy_Q;
    Eigen::MatrixXd R;
    Eigen::Vector4d K;
    Eigen::Vector4d N;
    Eigen::Vector4d B;
    Eigen::Vector4d state_vct;
    Eigen::Vector4d desired_state;

    void fancify();

    void set_K();

    public:

    LQR(FileWriter * filewriter, Filter * filter, Identifier axis, bool is_demo/*, Eigen::Matrix<4,1> desired state*/);

    void recieve_data();

};

#endif //__LQR_H__