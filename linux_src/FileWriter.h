#ifndef FILEWRITER_H
#define FILEWRITER_H

//c++ includes
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <stdio.h>

enum Identifier {
	X,
	Y
};

class FileWriter{
	protected:
		std::chrono::_V2::system_clock::time_point starttime;

		std::stringstream CameraXStream;
		std::stringstream CameraYStream;
		std::stringstream FilterXStream;
		std::stringstream FilterYStream;
		std::stringstream ControllerXStream;
		std::stringstream ControllerYStream;
		std::stringstream UARTStream;

		const std::string CameraXFileName = "camera_x_output.csv";
		const std::string CameraYFileName = "camera_y_output.csv";
		const std::string FilterXFileName = "filter_x_output.csv";
		const std::string FilterYFileName = "filter_y_output.csv";
		const std::string ControllerXFileName = "controller_x_output.csv";
		const std::string ControllerYFileName = "controller_y_output.csv";
		const std::string UARTFileName = "uart_output.csv";

		float ControllerXValue;
		float ControllerYValue;

	public:
		//FileWriter(std::string path);
		FileWriter();
		~FileWriter();

		virtual void initFile(std::fstream &filestream, std::string filename);
		virtual void writeToFile(std::fstream &filestream, std::string filename, std::stringstream &stringstream);

		virtual float readControllerValues(Identifier identifier);

		virtual void writeCameraInfo(Identifier identifier, float position, float velocity);
		virtual void writeControllerInfo(Identifier identifier, float value);
		virtual void writeFilterInfo(Identifier identifier, float angle, float ang_vel, float position, float velocity);
		virtual void writeUARTInfo(float xForce, float yForce, uint16_t xLidarFiltered, uint16_t yLidarFiltered, uint16_t xLidarRaw, uint16_t yLidarRaw);

		virtual void handleCTRLC();
};		

#endif