#ifndef CONTROLLER_H
#define CONTROLLER_H

//c++ includes
#include <ctime>

//required header files
#include "FileWriter.h"
#include "UARTInterface.h"

class Controller{
	protected:
		FileWriter * filewriter;
		UARTInterface * uartinterface;

	public:
		Controller(FileWriter * filewriter, UARTInterface * uartinterface);
		~Controller();

		virtual void PublishValues();

		virtual void run();
};

#endif