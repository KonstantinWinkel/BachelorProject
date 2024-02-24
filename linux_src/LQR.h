#ifndef __LQR_H__
#define __LQR_H__
//../Github/BachelorProject/linux_src/
#include "Controller.h"
#include <Eigen/Dense>

//Linear quadratic regulator 

class LQR : public Controller{
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

    LQR();

    void recieve_data();

};

#endif //__LQR_H__