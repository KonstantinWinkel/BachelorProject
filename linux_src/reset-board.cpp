//c++ includes
#include <string>
#include <limits>

//required header files
#include "UARTInterface.h"

int main(int argc, char** argv){
    std::string deviceName = argv[1];

    UARTInterface UARTInterface(deviceName, 115200);

    UARTInterface.SetControllerValues(std::numeric_limits<float>::max(), 0);

    UARTInterface.SendValues();
}