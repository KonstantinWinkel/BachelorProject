#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <ctime>
#include <chrono>
#include "FileWriter.h"
#include "DataPuffer.h"

using namespace je;

class Controller{
	protected:
		FileWriter * filewriter;
		std::array<double,6> state;
		std::array<double,6> PID_CONST;
		RingBuffer<double> phi_integrator;
		RingBuffer<double> x_integrator;
		Identifier lever;
		double acceleration;
		double velocity;
		double pos;
		std::chrono::_V2::system_clock::time_point last_update;

	public:

		Controller(FileWriter * filewriter, Identifier lever, size_t phi_integrator_size = 10,size_t pos_integrator_size = 10);
		~Controller();

		virtual void update_state(std::array<double,4> filtered_data);

		virtual void update_pos();

		virtual void update_accel();

		virtual void PublishValues();

		virtual void recieve_data(std::array<double,4> filtered_data);
};

#endif