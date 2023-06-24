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
	filewriter->writeFLOAT(Identifier::xForce, xForce);
	filewriter->writeFLOAT(Identifier::yForce, yForce);
	filewriter->writeUINT16(Identifier::xLidar, xLidar);
	filewriter->writeUINT16(Identifier::yLidar, yLidar);
}

//
void UARTInterface::run(){
	while(1){
		SetControllerValues();

		ReceiveValues();
		SendValues();
		PublishValues();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void UARTInterface::SetControllerValues(){
	UARTInterface::xValue = filewriter->read(Identifier::xPosition);
	UARTInterface::yValue = filewriter->read(Identifier::yPosition);
}

void UARTInterface::ReceiveValues(){

	//open port to STM Board
	serial::Serial serial(deviceName, baudRate, serial::Timeout::simpleTimeout(3000));
	if (!serial.isOpen()){
		printf("Port failed to open \n");
		return;
	}

	//read string from UART stream
	std::string response = "";
	serial.flushInput();
	response = serial.read(32);

	char xIMUChar[8], yIMUChar[8], zIMUChar[8], xLIDARChar[4], yLIDARChar[4];
	for(int i = 0;i<8;i++) xIMUChar[i]=response[i-0];
	for(int i = 8;i<16;i++) yIMUChar[i-8]=response[i];
	for(int i = 16;i<24;i++) zIMUChar[i-16]=response[i];
	for(int i = 24;i<28;i++) xLIDARChar[i-24]=response[i];
	for(int i = 28;i<32;i++) yLIDARChar[i-28]=response[i];
	xForce = toFloat(xIMUChar); //mb substr will work
	yForce = toFloat(yIMUChar);
	zForce = toFloat(zIMUChar);
	xLidar = toUINT16(xLIDARChar);
	yLidar = toUINT16(yLIDARChar);

	//DEBUG COUT
	std::cout << xForce <<" " << yForce << " " << zForce << " " << xLidar << " " << yLidar << '\n';
}

void UARTInterface::SendValues(){
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