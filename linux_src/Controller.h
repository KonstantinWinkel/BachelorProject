#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "FileWriter.h"

class Controller{
	protected:
		FileWriter * filewriter;
		Identifier axis;
		bool is_demo;
		double acceleration;

	public:

		~Controller(){};

		virtual void PublishValues(){
            if(!is_demo) filewriter->writeControllerInfo(axis, acceleration);
        }

		virtual void recieve_data(std::array<double,4> filtered_data) = 0;
};

#endif //CONTROLLER_H