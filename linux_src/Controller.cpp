#include <thread>
#include "Controller.h"

#include "bachelor_debug.h"

#if defined(_DEBUG_CONTROLLER_ON_)
#define _debug_print_controller_(x) std::cout << x << std::endl
#else
#define _debug_print_controller_(x)
#endif

Controller::Controller(FileWriter * filewriter, CommBuffer<std::array<double,4>> * filtered_data, Identifier lever, size_t phi_integrator_size,size_t x_integrator_size): phi_integrator(phi_integrator_size), x_integrator(x_integrator_size){
	Controller::lever = lever;
	Controller::filewriter = filewriter;
	Controller::filtered_data = filtered_data;
	Controller::acceleration = 0;
	Controller::velocity = 0;
	Controller::pos = 0;
	local_data = {0,0,0,0};
	PID_CONST = {0,0,1,12.9526,-4.1416,1};
	state = {0,0,0,0,0,0};
	last_update = std::chrono::high_resolution_clock::now();
}

Controller::~Controller(){

}

void Controller::PublishValues(){
	//pass calculated values to the filewriter
	filewriter->writeControllerInfo(lever, pos);
}

void Controller::update_pos(){
	double d_t = (std::chrono::high_resolution_clock::now() - last_update).count()/1000; //time_point.count() returns ms. -> /1000
	pos += velocity*d_t + acceleration*d_t*d_t;
	velocity += acceleration*d_t;
}

void Controller::update_accel(){
	acceleration = 0;
	for(int i=0;i<6;i++){
		acceleration+=PID_CONST[i] * state[i];
	}
}

void Controller::update_state(){
	state[0] = local_data[0];
	state[1] = local_data[1];
	state[2] = x_integrator.get_sum();
	state[3] = local_data[2];
	state[4] = local_data[3];
	state[5] = phi_integrator.get_sum();
}

void Controller::run(){
	while(1){
		if(filtered_data->newData()){ //std::this_thread::yield(); would be the implementation if the controller were to publish acceleration (pos would have to be calculated by the board)
			filtered_data->get(local_data);
			pos = local_data[0];
			velocity = local_data[1];
			x_integrator.set(local_data[0]);
			phi_integrator.set(local_data[2]);
			update_state();
			update_accel();
		}
		update_pos();
		PublishValues();
        _debug_print_controller_("Controller macht dinge");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}