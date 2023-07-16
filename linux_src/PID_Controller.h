#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <ctime>
#include <chrono>
#include "FileWriter.h"
#include "DataPuffer.h"
#include "Controller.h"

using namespace je;

class PID_Controller: public Controller{
	protected:
		bool is_demo;
		std::array<double,6> state;
		std::array<double,6> PID_CONST;
		RingBuffer<double> phi_integrator;
		RingBuffer<double> x_integrator;
		double velocity;
		double pos;
		std::chrono::_V2::system_clock::time_point last_update;

	public:

		PID_Controller(FileWriter * filewriter, Filter * filter, Identifier axis,bool is_demo, size_t phi_integrator_size = 10,size_t pos_integrator_size = 10);

		void update_state();

		void print_controller_state();

		void update_accel();

		void recieve_data();
};

#endif //PID_CONTROLLER