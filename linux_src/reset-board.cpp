//c++ includes
#include <string>
#include <stdlib.h>
#include <limits>

//required header files
#include "UARTInterface.h"

void help(){
    std::cout << "Use this programm to directly set the position of the platform" << std::endl;
    std::cout << "Usage: ./reset-board <device_name> <position>" << std::endl;
    std::cout << "List of positions:" << std::endl;
    std::cout << " - 0: middle\n - 1: top left\n - 2: top right\n - 3: bottom right\n - 4: bottom left" << std::endl;
    exit(0);
}

int main(int argc, char** argv){
    std::string deviceName = argv[1];
    if(strcmp(argv[1], "-h") == 0) help();

    int position = strtol(argv[2], NULL, 10);

    UARTInterface UARTInterface(deviceName, 115200);

    UARTInterface.SetControllerValues(std::numeric_limits<float>::max(), position);

    UARTInterface.SendValues();
}