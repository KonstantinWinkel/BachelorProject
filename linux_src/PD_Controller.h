#ifndef P_CONTROLLER_H
#define P_CONTROLLER_H

#include "FileWriter.h"
#include "Controller.h"

class PD_Controller : public Controller{
    protected:
        double k[4];

    public:
        PD_Controller(double k[4]);

        void calculate_u(double x_hat[4], double& u);
};

#endif