//c++ includes
#include <cmath>
#include <stdio.h>
#include <iostream>
#include "thread"

//required header files
#include "DemoProgram.h"

DemoProgram::DemoProgram(FileWriter * filewriter){
    DemoProgram::filewriter = filewriter;
}

DemoProgram::~DemoProgram(){

}

void DemoProgram::PublishValues(){
    //pass calculated values to the filewriter
	filewriter->write(Identifier::xPosition, xMotorValue );
	filewriter->write(Identifier::yPosition, yMotorValue );
}

void DemoProgram::run(){
    //instant move to bottom right corner
    std::cout << "instant move to bottom right corner" << std::endl;
    xMotorValue = PositionToAngle(0);
    yMotorValue = PositionToAngle(0);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    //instant move to top left corner
    std::cout << "instant move to top left corner" << std::endl;
    xMotorValue = PositionToAngle(10);
    yMotorValue = PositionToAngle(10);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    //slow move to bottom left corner
    std::cout << "slow move to bottom left corner" << std::endl;
    for(float i = 10; i > 0; i -= 0.1){
        yMotorValue = PositionToAngle(i);
        PublishValues();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    yMotorValue = PositionToAngle(0);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    //slow move to bottom left corner
    std::cout << "slow move to top right corner" << std::endl;
    float xTemp = 10;
    float yTemp = 0;
    while(xTemp > 0 && yTemp < 10){
        xTemp -= 0.1;
        yTemp += 0.1;
        xMotorValue = PositionToAngle(xTemp);
        yMotorValue = PositionToAngle(yTemp);
        PublishValues();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    xMotorValue = PositionToAngle(0);
    yMotorValue = PositionToAngle(10);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    std::cout << "END OF DEMO PROGRAM" << std::endl;
    exit(0);
}

float DemoProgram::PositionToAngle(float position){
    //     converto to angle         Rad2Deg      map 0 - 90 to -30 - 60    
    return acosf32(0.5*position/5) * (180.0/M_PI) - 30;  // L = 5cm, reference equation (5)
}

