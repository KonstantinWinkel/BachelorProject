//required header files
#include "Controller.h"

Controller::Controller(FileWriter * filewriter, UARTInterface * uartinterface){
	Controller::filewriter = filewriter;
	Controller::uartinterface = uartinterface;
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