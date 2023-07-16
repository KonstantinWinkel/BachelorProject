#ifndef KF_H
#define KF_H

#include "Filter.h"
#include <Eigen/Dense>
#include <chrono>

#define g 9.81
#define L 0.3

class Kalman_Filter : public Filter{
    private: 
    double latency; //might be a dumb idea we will see /:
    double dt;
    double u;
    std::chrono::_V2::system_clock::time_point time_now;
    std::chrono::_V2::system_clock::time_point last_update;
    Eigen::Vector4d x_hat_prio; //Schätzung Messwert
    Eigen::Vector4d x_hat_pos; //Verbesserte Schätzung. man braucht glaube ich nur einen der beiden Werte aber sicher ist sicher :)
    Eigen::Vector2d z; //Durch Modell vorhergesagte Messung
    Eigen::Vector2d y; //Gemessener Zustand
    Eigen::Vector2d nu; //Innovation (Abweichung Messung und Vorhersage)
    Eigen::Matrix<double,2,4> H; //Outpit matrix TODO 4*2Matrix
    Eigen::Matrix4d F; //Jakobimartix aus der Zustandsgleichung
    Eigen::Matrix2d S; //Jakobimartix aus der Zustandsgleichung
    Eigen::Matrix4d P_pri; //Jakobimartix aus der Zustandsgleichung
    Eigen::Matrix4d P_pos; //Jakobimartix aus der Zustandsgleichung
    Eigen::Matrix2d R; //Rauschen der Messung
    Eigen::Matrix4d Q; //Rauschen des Modells
    Eigen::Matrix<double,4,2> K;

    void update_dt();
    public:
    Kalman_Filter( double latency = 135.0 /*latency in ms*/);

    void next_measurement(double y0, double y1,double state[4]);

    void update_F();

    void predict();

    void update();

    void update_angle(double angle, double u, double state[4]);

    void update_pos(uint16_t pos, double u, double state[4]);
};

#endif //KF_H