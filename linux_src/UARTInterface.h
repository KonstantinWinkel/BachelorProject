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

		double xForce;
		double yForce;
		double zForce;

		double xValue;
		double yValue;

	public:
		UARTInterface(FileWriter * filewriter, std::string deviceName, int baudRate);
		~UARTInterface();

		virtual void PublishValues();

		virtual void run();

		virtual void ReceiveIMUValues();
		virtual void SendControllerValues();

		virtual void SetControllerValues(double xValue, double yValue);
};

#endif