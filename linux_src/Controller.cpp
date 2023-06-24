#include <thread>
#include "Controller.h"

Controller::Controller(FileWriter * filewriter, CommBuffer<std::array<double,4>> * databuffer){
	Controller::filewriter = filewriter;
	Controller::databuffer = databuffer;
}

Controller::~Controller(){

}

void Controller::PublishValues(){
	//pass calculated values to the filewriter
	//filewriter->write(Identifier::xPosition, <VALUE> );
	//filewriter->write(Identifier::yPosition, <VALUE> );
}

void Controller::run(){
	while(!databuffer->newData()) std::this_thread::yield();	
}