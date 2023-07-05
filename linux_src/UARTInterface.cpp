//c++ things
#include <stdio.h>
#include <iostream>
#include "thread"
#include "../rodos_src/include/util.h"

//required header files
#include "UARTInterface.h"

UARTInterface::UARTInterface(FileWriter * filewriter,Filter * filter_x, Filter * filter_y, std::string deviceName, int baudRate){
	UARTInterface::filewriter = filewriter;
	UARTInterface::filter_x = filter_x;
	UARTInterface::filter_y = filter_y;
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
	filter_x->recieve_pos(xLidarRaw);
	filter_y->recieve_pos(yLidarRaw);
}

//
void UARTInterface::run(){
	while(1){
		SetControllerValues();

		ReceiveValues();
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
		response = serial.read(41); //40
	serial.close();	

	if(response[40] != '#'){
		std::cout << "message dropped: " << response << std::endl;
		return;
	}

	for(int i = 0;i<8;i++) xIMUChar[i]=response[i-0];
	for(int i = 8;i<16;i++) yIMUChar[i-8]=response[i];
	for(int i = 16;i<24;i++) zIMUChar[i-16]=response[i];
	for(int i = 24;i<28;i++) xLIDARFilteredChar[i-24]=response[i];
	for(int i = 28;i<32;i++) yLIDARFilteredChar[i-28]=response[i];
	for(int i = 32;i<36;i++) xLIDARRawChar[i-32]=response[i];
	for(int i = 36;i<40;i++) yLIDARRawChar[i-36]=response[i];
	xForce = toFloat(xIMUChar); //mb substr will work
	yForce = toFloat(yIMUChar);
	zForce = toFloat(zIMUChar);
	xLidarFiltered = toUINT16(xLIDARFilteredChar);
	yLidarFiltered = toUINT16(yLIDARFilteredChar);
	xLidarRaw = toUINT16(xLIDARRawChar);
	yLidarRaw = toUINT16(yLIDARRawChar);

	_debug_print_uart_(response << " " << xForce <<" " << yForce << " " << zForce << " " << xLidarFiltered << " " << yLidarFiltered << " " << xLidarRaw << " " << yLidarRaw);
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

	asprintf(&outputString, "X%sY%sT%s#", xString, yString, tString);

	size_t bytesWritten = serial.write(outputString);
	serial.close();

	//DEBUG COUT
	//std::cout << "Bytes sent: " << bytesWritten << " Message: " << outputString << std::endl;
}