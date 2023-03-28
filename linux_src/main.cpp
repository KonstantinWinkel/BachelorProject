//c++ includes
#include <thread>

//required header files
#include "FileWriter.h"
#include "ImageProcessing.h"

int main(int argc, char** argv){

	FileWriter filewriter; 

	ImageProcessing x(2, "X", &filewriter, Identifier::xAngle, Identifier::xVelocity);
	ImageProcessing y(4, "Y", &filewriter, Identifier::yAngle, Identifier::yVelocity);
	
	std::thread xThread(&ImageProcessing::run, x);
	std::thread yThread(&ImageProcessing::run, y);

	xThread.join();
	yThread.join();
}