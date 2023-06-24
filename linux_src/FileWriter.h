#ifndef FILEWRITER_H
#define FILEWRITER_H

//c++ includes
#include <string>
#include <fstream>
#include <chrono>
#include <mutex>

enum Identifier {
	xAngle,
	yAngle,
	xVelocity,
	yVelocity,
	xPosition,
	yPosition,
	xForce,
	yForce,
	xLidar,
	yLidar
};

class FileWriter{
	protected:
		std::chrono::_V2::system_clock::time_point starttime;

		std::fstream file;

		std::mutex mutex;

		//ImageProcessing Values
		float xAngle;
		float xVelocity;
		float yAngle;
		float yVelocity;

		//Controller Values
		float xPosition = 0;
		float yPosition = 0;

		//IMU Values
		float xForce;
		float yForce;

		//LIDAR Values;
		uint16_t xLidar;
		uint16_t yLidar;

	public:
		//FileWriter(std::string path);
		FileWriter();
		~FileWriter();
		virtual void writeFLOAT(Identifier identifier, float value);
		virtual void writeUINT16(Identifier identifier, uint16_t value);
		virtual float read(Identifier identifier);
};

#endif