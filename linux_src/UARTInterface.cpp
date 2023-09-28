//c++ things
#include <stdio.h>
#include <iostream>
#include "thread"
#include "../rodos_src/include/util.h"

//required header files
#include "UARTInterface.h"

UARTInterface::UARTInterface(FileWriter * filewriter,Controller * controller_x, Controller * controller_y, std::string deviceName, int baudRate){
	UARTInterface::filewriter = filewriter;
	UARTInterface::controller_x = controller_x;
	UARTInterface::controller_y = controller_y;
	UARTInterface::baudRate = baudRate;
	UARTInterface::deviceName = deviceName;

	UARTInterface::xValue = 0;
	UARTInterface::yValue = 0;
	
	UARTInterface::lastIteration = std::chrono::high_resolution_clock::now();
}

UARTInterface::UARTInterface(std::string deviceName, int baudRate){
	UARTInterface::baudRate = baudRate;
	UARTInterface::deviceName = deviceName;

	UARTInterface::xValue = 0;
	UARTInterface::yValue = 0;

	UARTInterface::lastIteration = std::chrono::high_resolution_clock::now();
}

UARTInterface::~UARTInterface(){

}

void UARTInterface::PublishValues(){
	//pass calculated values to the filewriter
	filewriter->writeUARTInfo(xForce, yForce, xLidarFiltered, yLidarFiltered, xLidarRaw, yLidarRaw);
	controller_x->recieve_pos(xLidarRaw);
	controller_y->recieve_pos(yLidarRaw);
}

//
void UARTInterface::run(){
	while(1){
		SetControllerValues();

		//ReceiveValues();
		SendValues();
		PublishValues();

		//std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void UARTInterface::SetControllerValues(){
	xValue = filewriter->readControllerValues(Identifier::X);
	yValue = filewriter->readControllerValues(Identifier::Y);
}

void UARTInterface::SetControllerValues(float x, float y){
	xValue = x;
	yValue = y;
}

void UARTInterface::ReceiveValues(){

	//open port to STM Board

	serial::Serial serial(deviceName, baudRate, serial::Timeout::simpleTimeout(50));
	if (!serial.isOpen()){
		printf("Port failed to open \n");
		return;
	}

	//read string from UART stream
	std::string response = "";
	serial.flushInput();
	response = serial.read(20); //40
	serial.close();	

	if(response[19] != '#'){
		std::cout << "message dropped: " << response << std::endl;
		return;
	}

	std::cout << "message accepted: " << response << std::endl;

	char xResponse[8], yResponse[8];

	for(int i = 0;i<8;i++) xResponse[i]=response[i+2];
	for(int i = 0;i<8;i++) yResponse[i-8]=response[i+11];
	
	switch (response[0])
	{
		case '0':
			xForce = toFloat(xResponse);
			yForce = toFloat(yResponse);
			break;

		case '1':
			xLidarFiltered = toUINT16(xResponse);
			yLidarFiltered = toUINT16(yResponse);
			break;

		case '2':
			xLidarRaw = toUINT16(xResponse);
			yLidarRaw = toUINT16(yResponse);
			break;

		case '3':
			//TODO
			break;
		
		case '4':
			//TODO
			break;

		default:
			break;
	}

}

void UARTInterface::SendValues(){
	serial::Serial serial(deviceName, baudRate, serial::Timeout::simpleTimeout(3000));
	if (!serial.isOpen()) printf("Port failed to open \n");	
	serial.flushOutput();

	std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - lastIteration;
	lastIteration = std::chrono::high_resolution_clock::now();

	char * outputString;
	char xString[9] = ""; 
	char yString[9] = "";
	char tString[9] = "";
	toChars(xValue, xString);
	toChars(yValue, yString);
	toChars((float)(ms_double.count()/1000),tString);

	asprintf(&outputString, "X%sY%s#", xString, yString);

	size_t bytesWritten = serial.write(outputString);
	serial.close();

	//DEBUG COUT
	std::cout << "Bytes sent: " << bytesWritten << " Message: " << outputString << std::endl;
}