//c++ includes
#include <stdio.h>
#include <iostream>

//class header
#include "FileWriter.h"

//constructor, creates a new object
FileWriter::FileWriter(){
	starttime = std::chrono::high_resolution_clock::now();

	//check if file exists, if yes: clear, if no: create
	file.open("output.csv", std::ofstream::out | std::ofstream::trunc);
	
	if(!file){
		file.open("output.csv",  std::fstream::in | std::fstream::out | std::fstream::trunc);
		file << "\n";
		file.close();
	}
	else{
		file.close();
	}
}

//destructor left empty
FileWriter::~FileWriter(){

}

//takes a new value, replaces the corosponding old value and then writes the current time plus all values into a new line
void FileWriter::writeFLOAT(Identifier identifier, float value){

	FileWriter::mutex.lock();

	switch(identifier){
		case Identifier::xAngle: xAngle = value; break;
		case Identifier::yAngle: yAngle = value; break;
		case Identifier::xVelocity: xVelocity = value; break;
		case Identifier::yVelocity: yVelocity = value; break;
		case Identifier::xPosition: xPosition = value; break;
		case Identifier::yPosition: yPosition = value; break;
		case Identifier::xForce: xForce = value; break;
		case Identifier::yForce: yForce = value; break;
	}

	//FORMAT: csv
	//Time - xAngle - yAngle - xVelocity - yVelocity - xPosition - yPosition - xForce - yForce
	//Multiple files maybe?
	std::string line = "";

	//Add Time
	std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - starttime;
	line += std::to_string(ms_double.count()/1000) + ",";
	//Add Angles
	line += std::to_string(xAngle) + "," + std::to_string(yAngle) + ",";
	//Add Velocities
	line += std::to_string(xVelocity) + "," + std::to_string(yVelocity) + ",";
	//Add Positions
	line += std::to_string(xPosition) + "," + std::to_string(yPosition) + ",";
	//Add Force
	line += std::to_string(xForce) + "," + std::to_string(yForce) + ",";
	//Add Lidar
	line += std::to_string(xLidarFiltered) + "," + std::to_string(yLidarFiltered) + ",";
	line += std::to_string(xLidarRaw) + "," + std::to_string(yLidarRaw) + "\n";
	
	//Write line to File   
	file.open("output.csv", std::ios_base::app);
	file << line;
	file.close();

	FileWriter::mutex.unlock();

}

void FileWriter::writeUINT16(Identifier identifier, uint16_t value){
	FileWriter::mutex.lock();

	switch(identifier){
		case Identifier::xLidarFiltered: xLidarFiltered = value; break;
		case Identifier::yLidarFiltered: yLidarFiltered = value; break;
		case Identifier::xLidarRaw: xLidarRaw = value; break;
		case Identifier::yLidarRaw: yLidarRaw = value; break;
	}

	//FORMAT: csv
	//Time - xAngle - yAngle - xVelocity - yVelocity - xPosition - yPosition - xForce - yForce
	//Multiple files maybe?
	std::string line = "";

	//Add Time
	std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - starttime;
	line += std::to_string(ms_double.count()/1000) + ",";
	//Add Angles
	line += std::to_string(xAngle) + "," + std::to_string(yAngle) + ",";
	//Add Velocities
	line += std::to_string(xVelocity) + "," + std::to_string(yVelocity) + ",";
	//Add Positions
	line += std::to_string(xPosition) + "," + std::to_string(yPosition) + ",";
	//Add Force
	line += std::to_string(xForce) + "," + std::to_string(yForce) + "\n";
	//Add Lidar
	line += std::to_string(xLidarFiltered) + "," + std::to_string(yLidarFiltered) + ",";
	line += std::to_string(xLidarRaw) + "," + std::to_string(yLidarRaw) + "\n";
	
	//Write line to File   
	file.open("output.csv", std::ios_base::app);
	file << line;
	file.close();

	FileWriter::mutex.unlock();
	
}

float FileWriter::read(Identifier identifier){
	switch(identifier){
		case Identifier::xAngle: return xAngle;
		case Identifier::yAngle: return yAngle;
		case Identifier::xVelocity: return xVelocity;
		case Identifier::yVelocity: return yVelocity;
		case Identifier::xPosition: return xPosition;
		case Identifier::yPosition: return yPosition;
		case Identifier::xForce: return xForce;
		case Identifier::yForce: return yForce;
	}
	return -1;
}