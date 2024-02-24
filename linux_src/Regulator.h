#ifndef REGULATOR_H
#define REGULATOR_H

#include "Controller.h"
#include "Pass_filter.h"
#include "FileWriter.h"

class Regulator{
    
    private:

	FileWriter* filewriter;
	Identifier axis;
    
    Filter* filter;
    Controller* controller;

    double x_hat[4] = {0,0,0,0};


    bool is_demo;

    inline virtual void publish(){
        if(!is_demo) filewriter->writeControllerInfo(axis,u);
    }

    public:

    void set_filter(Filter* filter){
        this->filter = filter;
    }

    double u = 0;
    
    Regulator(Filter* filter, Controller* controller, FileWriter * Filewriter, Identifier axis, bool is_demo = false){
        Regulator::filter = filter;
        Regulator::controller = controller;
        Regulator::filewriter = filewriter;
        Regulator::axis = axis;
        Regulator::is_demo = is_demo;
    }

    void recieve_angle(double theta){
        filter->recieve_theta(theta,u);
        filter->get_state(x_hat);
        controller->calculate_u(x_hat,u);
        //cast (float) u bisschen unnötig imo
        publish();
    }

    void recieve_pos(double p){
        filter->recieve_p(p,u);
        filter->get_state(x_hat);
        controller->calculate_u(x_hat,u);
        //cast (float) u bisschen unnötig imo
        publish();
    }

    /*//only in the Simulation. because of the different sensor Frequencies y doesn't get updated at once.
    void recieve_y(double p, double theta){
        filter->recieve_y(p,theta,u);
        filter->get_state(x_hat);
        controller->calculate_u(x_hat,u);
        //this will always be ignored, because recieve_y doesnt get called outside of the simulation, but oh well
        publish();
    }*/

    //recieves the state without any measurement noise added to test the controller.
    void recieve_state(double state[4]){
        controller->calculate_u(state,u);
        publish();
    }


    void get_x_hat(double x_hat[4]){
        for(int i = 0;i<4;i++)
            x_hat[i] = this->x_hat[i];
    }
};

#endif //REGULATOR_H