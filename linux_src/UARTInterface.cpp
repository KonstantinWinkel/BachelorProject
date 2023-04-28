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
		ReceiveIMUValues();
		SendControllerValues();
		PublishValues();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void UARTInterface::SetControllerValues(double xValue, double yValue){
	UARTInterface::xValue = xValue;
	UARTInterface::yValue = yValue;
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
	serial.flushOutput();

	char * outputString;
	char xString[9] = ""; 
	char yString[9] = "";
	toChars(xValue, xString);
	toChars(yValue, yString); 

	asprintf(&outputString, "X%sY%s#", xString, yString);

	size_t bytesWritten = serial.write(outputString);
	std::cout << "Bytes sent: " << bytesWritten << " Message: " << outputString << std::endl;
}

float UARTInterface::toFloat(char* c){
	long floatBits = 0;

	for(int i = 0; i < 8; i++){
		long currentNumber = 0;
		currentNumber = c[i];
		currentNumber -= 97;

		floatBits += currentNumber << i*4;
	}
	
	float result = *(float*)&floatBits;
    return result;
}

void UARTInterface::toChars(float f, char * result){
	long floatBits = *(long*)&f;
	for(int i = 0; i < 8; i++){
		int currentNumber = 0; // from 0 to 16;

		for(int j = 0; j < 4; j++){
			if((floatBits >> (j + i*4))& 1U){
				currentNumber |= 1U << j;
			}
		}	
		currentNumber += 97;

		result[i] = currentNumber;
	}

	result[8] = '\0';
}