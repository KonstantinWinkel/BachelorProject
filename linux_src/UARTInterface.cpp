//required header files
#include "UARTInterface.h"

UARTInterface::UARTInterface(FileWriter * filewriter){
	UARTInterface::filewriter = filewriter;
}

UARTInterface::~UARTInterface(){

}

void UARTInterface::PublishValues(){
	//pass calculated values to the filewriter
	//filewriter->write(Identifier::xForce, <VALUE> );
	//filewriter->write(Identifier::yForce, <VALUE> );
}

void UARTInterface::run(){
	
}