#include "LQR.h"

#define g 9.81 
#define pi 3.1415 //wieso

LQR::LQR(){


    double L = 0.3; //Length of the pencils lever to it's center of Mass

/*Cost Matrices (can be modified)*/
    //state cost
    LQR::Q<<1, 0, 0, 0,
            0, 0.01, 0, 0,
            0, 0, 1, 0, 
            0, 0, 0, 100; 
    //actuator cost
    LQR::R << 1;
    //Time Horizon 
    LQR::N << 0,0,0,0;

/*--------------------------------*/

    LQR::A<<0, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 1,
            0, 0, g/L, 0; //Faktor vor Abl

    LQR::B<< 0, 1, 0, 1/L;

    LQR::desired_state << 0, 0, pi, 0;

    set_K();
}


void LQR::set_K(){
    fancify();
    Eigen::Matrix<double,8,8> H;
    H.block(0,0,4,4) = A;
    H.block(4,0,8,4) = -B*(R.inverse())*B.transpose();
    H.block(0,4,4,8) = -Q;
    H.block(4,4,8,8) = -A.transpose();
}

void LQR::fancify(){
    fancy_A = A - B*(R.inverse())*N.transpose();
    fancy_Q = Q - N*(R.inverse())*N.transpose();
}

void LQR::recieve_data(){
    // didnt work ):
}
