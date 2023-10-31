#ifndef DEMOPROGRAM_H
#define DEMOPROGRAM_H

//c++ includes
#include <ctime>

//reqired header files
#include "FileWriter.h"
//#include "UARTInterface.h"
#include "bachelor_debug.h"

#if defined(_DEBUG_DEMO_ON_)
#define _debug_print_demo_(x) std::cout << x << std::endl
#else
#define _debug_print_demo_(x)
#endif

class DemoProgram {
    protected:
        FileWriter * filewriter;
        //UARTInterface * uartinterface;

        float xMotorValue = 0;
        float yMotorValue = 0;

    public:
        DemoProgram(FileWriter * filewriter);
        ~DemoProgram();

        virtual void PublishValues();
        virtual void run();

        //utility methods
        virtual float PositionToAngle(float position);
        virtual void setAndWrite(float x, float y);

        virtual void bigDemo();

        virtual void circle();

        virtual void newDemo();
};

#endif