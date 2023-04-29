//required header files
#include "Controller.h"

//removed UARTInterface reference, grab newest values from Filewriter using filewriter->read(Identifier::<WHATEVER>)

Controller::Controller(FileWriter * filewriter){
	Controller::filewriter = filewriter;
}

Controller::~Controller(){

}

void Controller::PublishValues(){
	//pass calculated values to the filewriter
	//filewriter->write(Identifier::xPosition, <VALUE> );
	//filewriter->write(Identifier::yPosition, <VALUE> );
}

void Controller::run(){

}