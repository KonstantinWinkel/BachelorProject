#ifndef UARTINTERFACE_H
#define UARTINTERFACE_H

//required header files
#include "FileWriter.h"

class UARTInterface{
	protected:
		FileWriter * filewriter;

	public:
		UARTInterface(FileWriter * filewriter);
		~UARTInterface();

		virtual void PublishValues();

		virtual void run();
};

#endif