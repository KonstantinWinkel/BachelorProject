#ifndef __KF_H__
#define __KF_H__

#include "Filter.h"
#include <Eigen/Dense>

#define g 9.81

#ifndef L
#define L 0.3
#endif

class Extended_Kalman_Filter : public Filter{
    protected: 
    double dt;
    double u;
    Eigen::Matrix3d ny_u; //input rauschen

    double last_update;
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
    Eigen::Matrix<double,4,3> Fu; //Wird verwendet um Q zu aktualisieren

    public:

    Extended_Kalman_Filter();

    void update_Q();

    void update_F();

    void predict();

    void update();

    void recieve_pos(double p, double u);

    void recieve_angle(double theta, double u);

    void recieve_y(double p, double theta, double u);
};

#endif //__KF_H__