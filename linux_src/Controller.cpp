//required header files
#include "Controller.h"

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