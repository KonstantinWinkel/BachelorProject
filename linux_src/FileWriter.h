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
	yForce
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

	public:
		//FileWriter(std::string path);
		FileWriter();
		~FileWriter();
		virtual void write(Identifier identifier, float value);
		virtual float read(Identifier identifier);
};

#endif