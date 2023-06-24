#ifndef DEMOPROGRAM_H
#define DEMOPROGRAM_H

//c++ includes
#include <ctime>

//reqired header files
#include "FileWriter.h"
#include "UARTInterface.h"

class DemoProgram {
    protected:
        FileWriter * filewriter;
        UARTInterface * uartinterface;

        float xMotorValue = 0;
        float yMotorValue = 0;

    public:
        DemoProgram(FileWriter * filewriter);
        ~DemoProgram();

        virtual void PublishValues();
        virtual void run();

        //utility methods
        virtual float PositionToAngle(float position);

        virtual void bigDemo();

        virtual void circle();
};

#endif