//c++ includes
#include <cmath>
#include <stdio.h>
#include <iostream>
#include "thread"

//required header files
#include "DemoProgram.h"

#include "bachelor_debug.h"

#if defined(_DEBUG_DEMO_ON_)
#define _debug_print_demo_(x) std::cout << x << std::endl
#else
#define _debug_print_demo_(x)
#endif

DemoProgram::DemoProgram(FileWriter * filewriter){
    DemoProgram::filewriter = filewriter;
}

DemoProgram::~DemoProgram(){

}

void DemoProgram::PublishValues(){
    //pass calculated values to the filewriter
	filewriter->writeControllerInfo(Identifier::X, xMotorValue);
	filewriter->writeControllerInfo(Identifier::Y, yMotorValue);
    _debug_print_demo_("X: " << xMotorValue << " Y: " << yMotorValue);
}

void DemoProgram::run(){
   bigDemo();
}

float DemoProgram::PositionToAngle(float position){
    //     converto to angle                       Rad2Deg        
    //return (2.0/3.0) * (acosf32(0.5*position/5.0) * (180.0/M_PI));  // L = 5cm, reference equation (5)
    return (position-5.0f)*(50.0f/5.0f);
}

void DemoProgram::bigDemo(){
    float xTemp = 0;
    float yTemp = 0;
    //start in middle
    xMotorValue = PositionToAngle(7);
    yMotorValue = PositionToAngle(3);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    //instant move to bottom right corner
    std::cout << "instant move to bottom right corner" << std::endl;
    xMotorValue = PositionToAngle(3);
    yMotorValue = PositionToAngle(3);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    //instant move to top left corner
    std::cout << "instant move to top left corner" << std::endl;
    xMotorValue = PositionToAngle(7);
    yMotorValue = PositionToAngle(7);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    
    //slow move to bottom left corner
    std::cout << "slow move to bottom left corner" << std::endl;
    yTemp = 7;
    while(yTemp > 3){
        yMotorValue = PositionToAngle(yTemp);
        yTemp -= 0.005;
        PublishValues();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    yMotorValue = PositionToAngle(3);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    //slow move to bottom left corner
    std::cout << "slow move to top right corner" << std::endl;
    xTemp = 6;
    yTemp = 4;
    while(xTemp > 3 && yTemp < 7){
        xMotorValue = PositionToAngle(xTemp);
        yMotorValue = PositionToAngle(yTemp);
        xTemp -= 0.005;
        yTemp += 0.005;
        PublishValues();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    xMotorValue = PositionToAngle(3);
    yMotorValue = PositionToAngle(7);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    //return to middle
    xMotorValue = PositionToAngle(7);
    yMotorValue = PositionToAngle(3);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    std::cout << "END OF DEMO PROGRAM" << std::endl;
    exit(0);
}

void DemoProgram::circle(){
    float temp = 0;

    while(true){
        xMotorValue = PositionToAngle(5 + 5 * cos(temp));
        yMotorValue = PositionToAngle(5 + 5 * sin(temp));
        temp += 0.001;
        PublishValues();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
