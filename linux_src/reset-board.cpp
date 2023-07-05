//c++ includes
#include <string>
#include <stdlib.h>
#include <limits>

//required header files
#include "UARTInterface.h"

int main(int argc, char** argv){
    std::string deviceName = argv[1];
    int position = strtol(argv[2], NULL, 10);

    UARTInterface UARTInterface(deviceName, 115200);

    UARTInterface.SetControllerValues(std::numeric_limits<float>::max(), position);

    UARTInterface.SendValues();
}