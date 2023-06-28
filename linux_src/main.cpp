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
	std::string deviceName = "/dev/ttyACM0"; // for example "/dev/ttyACM0"
	std::string programMode = "exp";
	//std::string deviceName = argv[1]; // for example "/dev/ttyACM0"
	//std::string programMode = argv[2];
	std::cout << "Program Mode: " << programMode << std::endl;

	if(programMode.compare("demo") != 0 && programMode.compare("exp") != 0){
		std::cerr<< "\033[1;31mERROR: INVALID PROGRAM MODE\033[0m" << std::endl;
		return -1;
	}

	//Object Creation
	FileWriter filewriter;

	//Create CommBuffers for Communication
	double phiX,phiY;
	CommBuffer<double> ComBufPhiX; //angle sent by imageprocessing and recieved by filter
	CommBuffer<double> ComBufPhiY;

	uint16_t lidarX,lidarY;
	CommBuffer<uint16_t> ComBuffLidarX; //Lidar value sent by UartInterface, recieved by filter
	CommBuffer<uint16_t> ComBuffLidarY;

	std::array<double,4> data_x = {0,0,0,0};
	std::array<double,4> data_y = {0,0,0,0};
	CommBuffer<std::array<double,4>> filtered_data_x; //Filtered data sent by filter, recieved by controller
	CommBuffer<std::array<double,4>> filtered_data_y;

	UARTInterface uartinterface(&filewriter, &ComBuffLidarX, &ComBuffLidarY, deviceName, 115200);

	Controller controllerX(&filewriter,&filtered_data_x,Identifier::xPosition);
	Controller controllerY(&filewriter,&filtered_data_y,Identifier::yPosition);

	Filter filterX(&filtered_data_x, &ComBuffLidarX, &ComBufPhiX);
	Filter filterY(&filtered_data_y, &ComBuffLidarY, &ComBufPhiY);

	DemoProgram demoprogram(&filewriter);

	ImageProcessing x(0, "X", &filewriter, &ComBufPhiX, Identifier::xAngle, Identifier::xVelocity);
	ImageProcessing y(2, "Y", &filewriter, &ComBufPhiY, Identifier::yAngle, Identifier::yVelocity);

	//Creation of required threads
	std::thread xThread(&ImageProcessing::run, x);
	std::thread yThread(&ImageProcessing::run, y);
	std::thread uartThread(&UARTInterface::run, uartinterface);

	
	//conditional creation of controller and demo threads with join
	//if(programMode.compare("exp")==0) {
		std::thread controllerThreadX(&Controller::run, controllerX);
		std::thread controllerThreadY(&Controller::run, controllerY);
		std::thread filterThreadX(&Filter::run, filterX);
		std::thread filterThreadY(&Filter::run, filterY);
		
		controllerThreadX.join();
		controllerThreadY.join();
		filterThreadX.join();
		filterThreadY.join();
	//}

	if(programMode.compare("demo")==0){
		std::thread demoThread(&DemoProgram::run, demoprogram);
		demoThread.join();
	}


	//join all required threads
	xThread.join();
	yThread.join();
	uartThread.join();
}