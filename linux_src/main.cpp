//c++ includes
#include <thread>
#include <stdio.h>
#include <string>

//required header files
#include "FileWriter.h"
#include "DataPuffer.h"
#include "ImageProcessing.h"
#include "UARTInterface.h"
#include "Controller.h"
#include "Filter.h"
#include "DemoProgram.h"

int main(int argc, char** argv){

	//Setup
	//std::string deviceName = "/dev/ttyACM0"; // for example "/dev/ttyACM0"
	//std::string programMode = "demo";
	std::string deviceName = argv[1]; // for example "/dev/ttyACM0"
	std::string programMode = argv[2];
	std::cout << "Program Mode: " << programMode << std::endl;

	bool is_demo = false;

	if(programMode.compare("demo") != 0 && programMode.compare("exp") != 0){
		std::cerr<< "\033[1;31mERROR: INVALID PROGRAM MODE\033[0m" << std::endl;
		return -1;
	}

	is_demo = (programMode.compare("demo")==0);


	//Object Creation
	FileWriter filewriter;

	Controller controllerX(&filewriter, Identifier::X, is_demo);
	Controller controllerY(&filewriter, Identifier::Y, is_demo);

	Filter filterX(&controllerX);
	Filter filterY(&controllerY);

	UARTInterface uartinterface(&filewriter, &filterX, &filterY, deviceName, 115200);

	ImageProcessing x(6, "X", &filewriter, &filterX, Identifier::X);
	ImageProcessing y(2, "Y", &filewriter, &filterY, Identifier::Y);

	//Creation of required threads
	std::thread xThread(&ImageProcessing::run, x);
	std::thread yThread(&ImageProcessing::run, y);
	std::thread uartThread(&UARTInterface::run, uartinterface);

	if(is_demo) {
		DemoProgram demoprogram(&filewriter);

		std::thread demoThread(&DemoProgram::run, demoprogram);
		is_demo = true;
		demoThread.join();
	}

	//join all required threads
	xThread.join();
	yThread.join();
	uartThread.join();
}