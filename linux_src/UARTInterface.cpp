//c++ things
#include <stdio.h>
#include <iostream>
#include "thread"
#include "../rodos_src/include/util.h"

//required header files
#include "UARTInterface.h"

UARTInterface::UARTInterface(FileWriter * filewriter, std::string deviceName, int baudRate){
	UARTInterface::filewriter = filewriter;
	UARTInterface::deviceName = deviceName;
	UARTInterface::baudRate = baudRate;

	UARTInterface::xValue = 0;
	UARTInterface::yValue = 0;
}

UARTInterface::~UARTInterface(){

}

void UARTInterface::PublishValues(){
	//pass calculated values to the filewriter
	filewriter->write(Identifier::xForce, xForce);
	filewriter->write(Identifier::yForce, yForce);
}

//
void UARTInterface::run(){
	while(1){
		SetControllerValues();

		ReceiveIMUValues();
		SendControllerValues();
		PublishValues();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void UARTInterface::SetControllerValues(){
	UARTInterface::xValue = filewriter->read(Identifier::xPosition);
	UARTInterface::yValue = filewriter->read(Identifier::yPosition);
}

void UARTInterface::ReceiveIMUValues(){

	//open port to STM Board
	serial::Serial serial(deviceName, baudRate, serial::Timeout::simpleTimeout(3000));
	if (!serial.isOpen()){
		printf("Port failed to open \n");
		return;
	}

	//read string from UART stream
	std::string response = "";
	serial.flushInput();
	response = serial.read(24);

	char xChar[8], yChar[8], zChar[8];
	for(int i = 0;i<8;i++) xChar[i] = response[i];
	for(int i = 8;i<16;i++) yChar[i-8] =response[i];
	for(int i = 16;i<24;i++) zChar[i-16]=response[i];
	xForce = toFloat(xChar); //mb substr will work
	yForce = toFloat(yChar);
	zForce = toFloat(zChar);

	//DEBUG COUT
	//std::cout << xForce <<" " << yForce << " " <<zForce << '\n';
}

void UARTInterface::SendControllerValues(){
	serial::Serial serial(deviceName, baudRate, serial::Timeout::simpleTimeout(3000));
	if (!serial.isOpen()) printf("Port failed to open \n");	
	serial.flushOutput();

	char * outputString;
	char xString[9] = ""; 
	char yString[9] = "";
	toChars(xValue, xString);
	toChars(yValue, yString); 

	asprintf(&outputString, "X%sY%s#", xString, yString);

	size_t bytesWritten = serial.write(outputString);

	//DEBUG COUT
	//std::cout << "Bytes sent: " << bytesWritten << " Message: " << outputString << std::endl;
}