#ifndef P_CONTROLLER_H
#define P_CONTROLLER_H

#include "FileWriter.h"
#include "Controller.h"

class P_Controller : public Controller{
    protected:
        std::array<double, 4> state;
        double P = 45.0;
        double I = 0;
        double D = 0.0;//0.001;

        double sum_of_angles = 0;

    public:
        P_Controller(FileWriter * filewriter, Identifier axis, bool is_demo);

        void update_accell();

        void recieve_data(std::array<double, 4> filtered_data);
};

#endif