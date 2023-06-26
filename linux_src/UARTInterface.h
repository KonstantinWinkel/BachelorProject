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

		uint16_t xLidarFiltered;
		uint16_t yLidarFiltered;

		uint16_t xLidarRaw;
		uint16_t yLidarRaw;

	public:
		UARTInterface(FileWriter * filewriter, std::string deviceName, int baudRate);
		~UARTInterface();

		virtual void PublishValues();

		virtual void run();

		virtual void ReceiveValues();
		virtual void SendValues();

		virtual void SetControllerValues();

		//utility methods
		//virtual float toFloat(char* c);
		//virtual void toChars(float f, char * result);
};

#endif