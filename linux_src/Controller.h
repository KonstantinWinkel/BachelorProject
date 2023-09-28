#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "FileWriter.h"
#include "Filter.h"
#include "fstream"

class Controller{
	protected:
		Filter * filter;
		FileWriter * filewriter;
		Identifier axis;
		double filtered_state[4];
		bool is_demo;
		double acceleration;

	public:

		~Controller(){};

		Identifier get_axis(){
			return axis;
		}

		virtual void PublishValues(){
            if(!is_demo) filewriter->writeControllerInfo(axis, acceleration);
        }

		virtual void recieve_angle(double angle){
			filter->update_angle(angle,acceleration,filtered_state);
			this->recieve_data();
		}

		virtual void recieve_pos(uint16_t pos){
			filter->update_pos(pos,acceleration,filtered_state);
			this->recieve_data();
		}

		virtual void recieve_data() = 0;
};

#endif //CONTROLLER_H