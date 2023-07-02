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

	if(programMode.compare("demo") != 0 && programMode.compare("exp") != 0){
		std::cerr<< "\033[1;31mERROR: INVALID PROGRAM MODE\033[0m" << std::endl;
		return -1;
	}

	//Object Creation
	FileWriter filewriter;

	Controller controllerX(&filewriter,Identifier::X);
	Controller controllerY(&filewriter,Identifier::Y);

	Filter filterX(&controllerX);
	Filter filterY(&controllerY);

	UARTInterface uartinterface(&filewriter, &filterX, &filterY, deviceName, 115200);
	
	DemoProgram demoprogram(&filewriter);

	ImageProcessing x(2, "X", &filewriter, &filterX, Identifier::X);
	ImageProcessing y(6, "Y", &filewriter, &filterY, Identifier::Y);

	//Creation of required threads
	std::thread xThread(&ImageProcessing::run, x);
	std::thread yThread(&ImageProcessing::run, y);
	std::thread uartThread(&UARTInterface::run, uartinterface);

	
	//conditional creation of controller and demo threads with join
	if(programMode.compare("exp")==0) {
		//TODO fix programmode (demo will not work because the Controller isn't thread based anymore -> it cannot be turned off and will interfere with the demo program)
		//std::thread controllerThreadX(&Controller::run, controllerX);
		//std::thread controllerThreadY(&Controller::run, controllerY);
		//std::thread filterThreadX(&Filter::run, filterX);
		//std::thread filterThreadY(&Filter::run, filterY);
		//
		//controllerThreadX.join();
		//controllerThreadY.join();
		//filterThreadX.join();
		//filterThreadY.join();
	}

	if(programMode.compare("demo")==0){
		std::thread demoThread(&DemoProgram::run, demoprogram);
		demoThread.join();
	}


	//join all required threads
	xThread.join();
	yThread.join();
	uartThread.join();
}