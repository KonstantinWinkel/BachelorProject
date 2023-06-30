#ifndef UARTINTERFACE_H
#define UARTINTERFACE_H

//required header files
#include "FileWriter.h"
#include "../serial/include/serial/serial.h"

#include "Filter.h"

#include "bachelor_debug.h"

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

		Filter * filter_x;
		Filter * filter_y;

		float xForce;
		float yForce;
		float zForce;

		float xValue; // currently changed to float, was double
		float yValue; // maybe float is enough accuarcy?

		uint16_t xLidarRaw;
		uint16_t yLidarRaw;

		uint16_t xLidarFiltered;
		uint16_t yLidarFiltered;

	public:
		UARTInterface(FileWriter * filewriter,Filter * filter_x, Filter * filter_y, std::string deviceName, int baudRate);
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