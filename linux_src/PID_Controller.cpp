#include <thread>
#include "PID_Controller.h"

#include "bachelor_debug.h"

#if defined(_DEBUG_CONTROLLER_ON_)
#define _debug_print_controller_(x) std::cout << x << std::endl
#else
#define _debug_print_controller_(x)
#endif

PID_Controller::PID_Controller(FileWriter * filewriter, Filter * filter, Identifier axis,bool is_demo , size_t phi_integrator_size,size_t x_integrator_size): phi_integrator(phi_integrator_size), x_integrator(x_integrator_size){
	Controller::filter = filter;
	Controller::is_demo = is_demo;
	Controller::axis = axis;
	Controller::filewriter = filewriter;
	Controller::acceleration = 0;
	PID_Controller::velocity = 0;
	PID_Controller::pos = 0;
	//PID_Controller::PID_CONST = {0,0,0,10.2970,-0.6419,0};
	PID_Controller::PID_CONST = {0,0,0,200.535,-0.6419,0};
	PID_Controller::state = {0,0,0,0,0,0};
	PID_Controller::last_update = std::chrono::high_resolution_clock::now();
}

void PID_Controller::print_controller_state(){
	std::stringstream s;
	for(int i = 0; i<5;i++){
		s << state[i] << ',';
	}
	s << state[5];
	std::cout << "Controller" << (axis?"X":"Y") << " state: " << s.str() << std::endl;
}

void PID_Controller::update_accel(){
	acceleration = 0;
	for(int i=0;i<6;i++){
		acceleration-=PID_CONST[i] * state[i];
	}
}

void PID_Controller::update_state(){
	state[0] = filtered_state[0] - (axis == Identifier::X ? 0.173169:0.0873933);
	state[1] = filtered_state[1];
	state[2] = x_integrator.get_sum();
	state[3] = filtered_state[2];
	state[4] = filtered_state[3];
	state[5] = phi_integrator.get_sum();
}

void PID_Controller::recieve_data(){
	pos = filtered_state[0];
	velocity = filtered_state[1];
	x_integrator.set(filtered_state[0] - axis == Identifier::X ? 0.173169:0.0873933);
	phi_integrator.set(filtered_state[2]);

	update_state();
	update_accel();
	PublishValues();

	print_controller_state();
	_debug_print_controller_("Controller recieved data");
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}