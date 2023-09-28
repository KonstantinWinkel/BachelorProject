#ifndef UARTINTERFACE_H
#define UARTINTERFACE_H

//required header files
#include "FileWriter.h"
#include "../serial/include/serial/serial.h"

#include "Controller.h"

#include "bachelor_debug.h"

#include <chrono>

#if defined(_DEBUG_UART_ON_)
#define _debug_print_uart_(x) std::cout << x << std::endl
#else
#define _debug_print_uart_(x)
#endif

using namespace je;

class UARTInterface{
	protected:
		FileWriter * filewriter;

		std::string deviceName;
		int baudRate;

		Controller * controller_x;
		Controller * controller_y;

		float xForce;
		float yForce;
		float zForce;

		float xValue; // currently changed to float, was double
		float yValue; // maybe float is enough accuarcy?

		int16_t xLidarRaw;
		int16_t yLidarRaw;

		int16_t xLidarFiltered;
		int16_t yLidarFiltered;

		std::chrono::_V2::system_clock::time_point lastIteration;

		char xIMUChar[8], yIMUChar[8], zIMUChar[8], xLIDARFilteredChar[4], yLIDARFilteredChar[4], xLIDARRawChar[4], yLIDARRawChar[4];

	public:
		UARTInterface(FileWriter * filewriter,Controller * controller_x, Controller * controller_y, std::string deviceName, int baudRate);
		UARTInterface(std::string deviceName, int baudRate);
		~UARTInterface();

		virtual void PublishValues();

		virtual void run();

		virtual void ReceiveValues();
		virtual void SendValues();

		virtual void SetControllerValues();
		virtual void SetControllerValues(float x, float y);

		//utility methods
		//virtual float toFloat(char* c);
		//virtual void toChars(float f, char * result);
};

#endif