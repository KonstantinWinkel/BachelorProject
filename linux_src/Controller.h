#ifndef CONTROLLER_H
#define CONTROLLER_H

//c++ includes
#include <ctime>

//required header files
#include "FileWriter.h"

class Controller{
    protected:
        FileWriter * filewriter;

    public:
        Controller(FileWriter * filewriter);
        ~Controller();

        virtual void PublishValues();

        virtual void run();
};

#endif