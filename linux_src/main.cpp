//c++ includes
#include <thread>
#include <stdio.h>

//required header files
#include "FileWriter.h"
#include "ImageProcessing.h"
#include "UARTInterface.h"
#include "Controller.h"

int main(int argc, char** argv){

	FileWriter filewriter;

	UARTInterface uartinterface(&filewriter, "/dev/ttyACM0", 115200);

	//Controller controller(&filewriter, &uartinterface);
	
	//ImageProcessing x(2, "X", &filewriter, Identifier::xAngle, Identifier::xVelocity);
	//ImageProcessing y(4, "Y", &filewriter, Identifier::yAngle, Identifier::yVelocity);
	
	//std::thread xThread(&ImageProcessing::run, x);
	//std::thread yThread(&ImageProcessing::run, y);
	std::thread uartThread(&UARTInterface::run, uartinterface);
	//std::thread controllerThread(&Controller::run, controller);

	//xThread.join();
	//yThread.join();
	uartThread.join();
	//controllerThread.join();

	//uartinterface.run();
}