//c++ includes
#include <thread>

//required header files
#include "FileWriter.h"
#include "ImageProcessing.h"

int main(int argc, char** argv){

    FileWriter filewriter; 

	ImageProcessing x(0, "X", &filewriter, Identifier::xAngle, Identifier::xVelocity);


    //x.run();
    //ImageProcessing y(3, "Y");
	
	std::thread xThread(&ImageProcessing::run, x);
    //std::thread yThread(&ImageProcessing::run, y);

	xThread.join();
    //yThread.join();
}