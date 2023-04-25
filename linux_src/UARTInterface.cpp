//c++ things
#include <stdio.h>
#include <iostream>
#include "thread"

//required header files
#include "UARTInterface.h"

UARTInterface::UARTInterface(FileWriter * filewriter, std::string deviceName, int baudRate){
	UARTInterface::filewriter = filewriter;
	UARTInterface::deviceName = deviceName;
	UARTInterface::baudRate = baudRate;
}

UARTInterface::~UARTInterface(){

}

void UARTInterface::PublishValues(){
	//pass calculated values to the filewriter
	filewriter->write(Identifier::xForce, xForce);
	filewriter->write(Identifier::yForce, yForce);
}

void UARTInterface::run(){
	while(1){
		ReceiveIMUValues();
		//SendControllerValues();
		PublishValues();
	}
}

void UARTInterface::SetControllerValues(double xValue, double yValue){

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
	response = serial.read(26);
	//std::cout << response << std::endl;
	
	//check if the string has right format
	//example: -X 0.921 -Y 3.987 -Z 3.571
	//future problem: what to do with negative numbers: HIER MUSS NOCH GEDENKT WERDEN
	int xpos = response.find("-X ");
	int ypos = response.find("-Y ");
	int zpos = response.find("-Z ");
	
	if(xpos == 0 && ypos == 9 && zpos == 18){
		xForce = std::stod(response.substr(3, 5));
		yForce = std::stod(response.substr(12, 5));
	}
	else{
		std::cout << "UART Message not in right format - skipping cycle" << std::endl;
	}
}

void UARTInterface::SendControllerValues(){
	serial::Serial serial(deviceName, baudRate, serial::Timeout::simpleTimeout(3000));
	if (!serial.isOpen()) printf("Port failed to open \n");	
	
}

void tempmethod(){
	serial::Serial serial("/dev/ttyACM0", 115200, serial::Timeout::simpleTimeout(3000));

	if (serial.isOpen())
    {
        printf("Port opened succesfully \n");
    }
    else
    {
        printf("Port failed to open \n");
    }
	serial.flushOutput();

	while(true){
		std::string response = "";
		std::cout << "String before read 1: " << response << std::endl; 
		//std::this_thread::sleep_for(std::chrono::milliseconds(20));
		serial.flushInput();
		response = serial.read(24);
		std::cout << response << std::endl;
		//std::cout << "String after read: " << response << std::endl;

	}
}