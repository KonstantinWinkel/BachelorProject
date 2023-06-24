//c++ includes
#include <thread>
#include <stdio.h>
#include <string>

//required header files
#include "FileWriter.h"
#include "ImageProcessing.h"
#include "UARTInterface.h"
#include "Controller.h"
#include "DemoProgram.h"

int main(int argc, char** argv){

	//Setup
	std::string deviceName = argv[1]; // for example "/dev/ttyACM0"
	std::string programMode = argv[2];
	std::cout << "Program Mode: " << programMode << std::endl;

	if(programMode.compare("demo") != 0 && programMode.compare("exp") != 0){
		std::cerr<< "\033[1;31mERROR: INVALID PROGRAM MODE\033[0m" << std::endl;
		return -1;
	}

	//Object Creation
	FileWriter filewriter;

	UARTInterface uartinterface(&filewriter, deviceName, 115200);

	Controller controller(&filewriter);
	DemoProgram demoprogram(&filewriter);
	
	ImageProcessing x(2, "X", &filewriter, Identifier::xAngle, Identifier::xVelocity);
	ImageProcessing y(6, "Y", &filewriter, Identifier::yAngle, Identifier::yVelocity);
	

	//Creation of required threads
	std::thread xThread(&ImageProcessing::run, x);
	std::thread yThread(&ImageProcessing::run, y);
	std::thread uartThread(&UARTInterface::run, uartinterface);

	//conditional creation of controller and demo threads with join
	if(programMode.compare("exp") == 0) {
		std::thread controllerThread(&Controller::run, controller);
		controllerThread.join();
	}

	if(programMode.compare("demo") == 0){
		std::thread demoThread(&DemoProgram::run, demoprogram);
		demoThread.join();
	}


	//join all required threads
	uartThread.join();
	xThread.join();
	yThread.join();
}