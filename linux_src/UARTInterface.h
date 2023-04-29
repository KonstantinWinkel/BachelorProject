#ifndef UARTINTERFACE_H
#define UARTINTERFACE_H

//required header files
#include "FileWriter.h"
#include "../serial/include/serial/serial.h"

class UARTInterface{
	protected:
		FileWriter * filewriter;

		std::string deviceName;
		int baudRate;

		float xForce;
		float yForce;
		float zForce;

		float xValue; // currently changed to float, was double
		float yValue; // maybe float is enough accuarcy?

	public:
		UARTInterface(FileWriter * filewriter, std::string deviceName, int baudRate);
		~UARTInterface();

		virtual void PublishValues();

		virtual void run();

		virtual void ReceiveIMUValues();
		virtual void SendControllerValues();

		virtual void SetControllerValues();

		//utility methods
		//virtual float toFloat(char* c);
		//virtual void toChars(float f, char * result);
};

#endif