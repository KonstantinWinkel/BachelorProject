#include <thread>
#include "Controller.h"

#include "bachelor_debug.h"

#if defined(_DEBUG_CONTROLLER_ON_)
#define _debug_print_controller_(x) std::cout << x << std::endl
#else
#define _debug_print_controller_(x)
#endif

Controller::Controller(FileWriter * filewriter, Identifier lever,bool is_demo , size_t phi_integrator_size,size_t x_integrator_size): phi_integrator(phi_integrator_size), x_integrator(x_integrator_size){
	Controller::is_demo = is_demo;
	Controller::lever = lever;
	Controller::filewriter = filewriter;
	Controller::acceleration = 0;
	Controller::velocity = 0;
	Controller::pos = 0;
	PID_CONST = {0,0,0,10.2970,-0.6419,0};
	state = {0,0,0,0,0,0};
	last_update = std::chrono::high_resolution_clock::now();
}

Controller::~Controller(){

}

void Controller::print_controller_state(){
	std::stringstream s;
	for(int i = 0; i<5;i++){
		s << state[i] << ',';
	}
	s << state[5];
	std::cout << "Controller state: " << s.str() << std::endl;
}

void Controller::PublishValues(){
	print_controller_state();
	std::cout << "published accel: " << acceleration << std::endl;
	filewriter->writeControllerInfo(lever, acceleration);
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

void Controller::update_state(std::array<double,4> filtered_data){
	state[0] = filtered_data[0] - lever == Identifier::X ? 0.173169:0.0873933;
	state[1] = filtered_data[1];
	state[2] = x_integrator.get_sum();
	state[3] = filtered_data[2];
	state[4] = filtered_data[3];
	state[5] = phi_integrator.get_sum();
}

void Controller::recieve_data(std::array<double,4> filtered_data){
	pos = filtered_data[0];
	velocity = filtered_data[1];
	x_integrator.set(filtered_data[0] - lever == Identifier::X ? 0.173169:0.0873933);
	phi_integrator.set(filtered_data[2]);
	update_state(filtered_data);
	update_accel();
	
	update_pos();
	if(!is_demo) PublishValues();
	_debug_print_controller_("Controller recieved data");
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}