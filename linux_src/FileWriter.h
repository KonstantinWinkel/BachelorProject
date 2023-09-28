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

		const std::string CameraXFileName = "../csv_files/camera_x_output.csv";
		const std::string CameraYFileName = "../csv_files/camera_y_output.csv";
		const std::string FilterXFileName = "../csv_files/filter_x_output.csv";
		const std::string FilterYFileName = "../csv_files/filter_y_output.csv";
		const std::string ControllerXFileName = "../csv_files/controller_x_output.csv";
		const std::string ControllerYFileName = "../csv_files/controller_y_output.csv";
		const std::string UARTFileName = "../csv_files/uart_output.csv";

		float ControllerXValue;
		float ControllerYValue;

		int UART_all_messages_in = 0;
		int UART_accepted_messages_in = 0;
		int UART_all_messages_out = 0;

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
		virtual void writeUARTInfo(float xForce, float yForce, int16_t xLidarFiltered, int16_t yLidarFiltered, int16_t xLidarRaw, int16_t yLidarRaw);
		virtual void writeUARTMessageInfo(int all_in, int acc_in, int all_out);

		virtual void handleCTRLC();
};		

#endif