//c++ includes
#include <stdio.h>
#include <iostream>

//class header
#include "FileWriter.h"

//constructor, creates a new object
FileWriter::FileWriter(){
	starttime = std::chrono::high_resolution_clock::now();

	//check if files exists, if yes: clear, if no: create
	initFile(CameraXFile, CameraXFileName);
	initFile(CameraYFile, CameraYFileName);
	initFile(FilterXFile, FilterXFileName);
	initFile(FilterYFile, FilterYFileName);
	initFile(ControllerXFile, ControllerXFileName);
	initFile(ControllerYFile, ControllerYFileName);
	initFile(UARTFile, UARTFileName);

	FileWriter::ControllerXValue = 0;
	FileWriter::ControllerYValue = 0;
}

//destructor left empty
FileWriter::~FileWriter(){

}

void FileWriter::initFile(std::fstream &filestream, std::string filename){
	filestream.open(filename, std::ofstream::out | std::ofstream::trunc);

	if(!filestream){
		filestream.open(filename,  std::fstream::in | std::fstream::out | std::fstream::trunc);
		filestream << "\n";
		filestream.close();
	}
	else filestream.close();
}

void FileWriter::writeToFile(std::fstream &filestream, std::string filename, std::string line){
	filestream.open(filename, std::ios_base::app);
	filestream << line;
	filestream.close();
}

void FileWriter::readControllerValues(float &xValue, float &yValue){
	xValue = FileWriter::ControllerXValue;
	yValue = FileWriter::ControllerYValue;
}

void FileWriter::writeCameraInfo(Identifier identifier, float position, float velocity){

	std::string line = "";

	//Add Time, angular position and angular velocity
	std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - starttime;
	line += std::to_string(ms_double.count()/1000) + ",";
	line += std::to_string(position) + "," + std::to_string(velocity) + "\n";

	//Write line to File
	switch (identifier){
		case Identifier::X: writeToFile(CameraXFile, CameraXFileName, line); break;
		case Identifier::Y: writeToFile(CameraYFile, CameraYFileName, line); break;
		default: break;
	}

}

void FileWriter::writeControllerInfo(Identifier identifier, float value){
	
	std::string line = "";

	//Add Time and controller value
	std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - starttime;
	line += std::to_string(ms_double.count()/1000) + ",";
	line += std::to_string(value) + "\n";

	//Write line to File
	switch (identifier){
		case Identifier::X: 
			writeToFile(ControllerXFile, ControllerXFileName, line);
			FileWriter::ControllerXValue = value;
			break;
		case Identifier::Y: 
			writeToFile(ControllerYFile, ControllerYFileName, line);
			FileWriter::ControllerYValue = value;
			break;
		default: break;
	}

}

void FileWriter::writeFilterInfo(Identifier identifier, float angle, float ang_vel, float position, float velocity){
	std::string line = "";

	//Add Time and controller value
	std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - starttime;
	line += std::to_string(ms_double.count()/1000) + ",";
	line += std::to_string(angle) + "," + std::to_string(ang_vel) + ",";
	line += std::to_string(position) + "," + std::to_string(velocity) + "\n";

	//Write line to File
	switch (identifier){
		case Identifier::X: writeToFile(FilterXFile, FilterXFileName, line); break;
		case Identifier::Y: writeToFile(FilterYFile, FilterYFileName, line); break;
		default: break;
	}
}

void FileWriter::writeUARTInfo(float xForce, float yForce, uint16_t xLidarFiltered, uint16_t yLidarFiltered, uint16_t xLidarRaw, uint16_t yLidarRaw){

	std::string line = "";

	//Add Time, Forces, filtered and raw Lidar data
	std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - starttime;
	line += std::to_string(ms_double.count()/1000) + ",";
	line += std::to_string(xForce) + "," + std::to_string(yForce) + ",";
	line += std::to_string(xLidarFiltered) + "," + std::to_string(yLidarFiltered) + ",";
	line += std::to_string(xLidarRaw) + "," + std::to_string(yLidarRaw) + "\n";

	//Write line to File
	writeToFile(UARTFile, UARTFileName, line);

}