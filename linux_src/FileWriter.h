#ifndef FILEWRITER_H
#define FILEWRITER_H

//c++ includes
#include <string>
#include <fstream>
#include <chrono>

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

		//ImageProcessing Values
		double xAngle;
		double xVelocity;
		double yAngle;
		double yVelocity;

		//Controller Values
		double xPosition;
		double yPosition;

		//IMU Values
		double xForce;
		double yForce;

	public:
		//FileWriter(std::string path);
		FileWriter();
		~FileWriter();
		virtual void write(Identifier identifier, double value);
		virtual double read(Identifier identifier);
};

#endif