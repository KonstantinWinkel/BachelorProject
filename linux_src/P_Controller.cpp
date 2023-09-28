#include "P_Controller.h"

P_Controller::P_Controller(FileWriter * filewriter, Identifier axis, bool is_demo){
    Controller::is_demo = is_demo;
    Controller::axis = axis;
    Controller::filewriter = filewriter;

    P_Controller::state = {0,0,0,0};
}

void P_Controller::update_accell(){
    Controller::acceleration = -P * state[0] - I * sum_of_angles - D * state[1];
    //std::cout << Controller::acceleration << std::endl;
    Controller::PublishValues();
}

void P_Controller::recieve_data(){
    for(int i = 0; i < 4; i++) P_Controller::state[i] = filtered_state[i];

    sum_of_angles += filtered_state[0];

    update_accell();
}