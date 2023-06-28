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
	filewriter->writeFLOAT(Identifier::xPosition, xMotorValue );
	filewriter->writeFLOAT(Identifier::yPosition, yMotorValue );
    std::cout << "X: " << xMotorValue << " Y: " << yMotorValue << std::endl;
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
    xMotorValue = PositionToAngle(6);
    yMotorValue = PositionToAngle(4);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    //instant move to bottom right corner
    std::cout << "instant move to bottom right corner" << std::endl;
    xMotorValue = PositionToAngle(4);
    yMotorValue = PositionToAngle(4);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    //instant move to top left corner
    std::cout << "instant move to top left corner" << std::endl;
    xMotorValue = PositionToAngle(6);
    yMotorValue = PositionToAngle(6);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    
    //slow move to bottom left corner
    std::cout << "slow move to bottom left corner" << std::endl;
    yTemp = 6;
    while(yTemp > 4){
        yMotorValue = PositionToAngle(yTemp);
        yTemp -= 0.005;
        PublishValues();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    yMotorValue = PositionToAngle(4);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    //slow move to bottom left corner
    std::cout << "slow move to top right corner" << std::endl;
    xTemp = 6;
    yTemp = 4;
    while(xTemp > 4 && yTemp < 6){
        xMotorValue = PositionToAngle(xTemp);
        yMotorValue = PositionToAngle(yTemp);
        xTemp -= 0.005;
        yTemp += 0.005;
        PublishValues();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    xMotorValue = PositionToAngle(4);
    yMotorValue = PositionToAngle(6);
    PublishValues();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    //return to middle
    xMotorValue = PositionToAngle(6);
    yMotorValue = PositionToAngle(4);
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
