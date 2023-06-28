//c++ includes
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <math.h>

void replace_value (float  &original, float replace_value, int &counter){
    original = replace_value;
    counter++;
    //std::cout << "hi " << address << std::endl;
}

int main(int argc, char** argv){
    //open file
    std::ifstream file(argv[1]);
    std::fstream outputFile;
    outputFile.open("output_cleanAF.csv" , std::ios::out);

    if (!outputFile) {
		std::cout << "File not created!" << std::endl;
        exit(1);
	}
    float threashholds[4] = {1000.0, 1000.0, 1000.0, 1000.0};
    float replace_values[4] = {90.0, 0.0 , 0.0, 0.0};

    int replace_counter = 0;
    int line_counter = 0;
    std::chrono::_V2::system_clock::time_point start_time = std::chrono::high_resolution_clock::now();

    std::string line;
    std::string newline;
    float time, xAngle, yAngle, xVelocity, yVelocity, xController, yController, xForce, yForce, xLidarFil, yLidarFil, xLidarRaw, yLidarRaw;
    
    while (file >> line){

        sscanf(line.c_str(), "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &time, &xAngle, &yAngle, &xVelocity, &yVelocity, &xController, &yController, &xForce, &yForce, &xLidarFil, &yLidarFil, &xLidarRaw, &yLidarRaw);
        //std::cout << xAngle << std::endl;
        if(std::fabs(xAngle) > threashholds[0]) replace_value(xAngle, replace_values[0], replace_counter);
        if(std::fabs(yAngle) > threashholds[0]) replace_value(yAngle, replace_values[0], replace_counter);

        if(std::fabs(xVelocity) > threashholds[1]) replace_value(xVelocity, replace_values[1], replace_counter);
        if(std::fabs(yVelocity) > threashholds[1]) replace_value(yVelocity, replace_values[1], replace_counter);

        if(std::fabs(xForce) > threashholds[2]) replace_value(xForce, replace_values[2], replace_counter);
        if(std::fabs(yForce) > threashholds[2]) replace_value(yForce, replace_values[2], replace_counter);

        if(std::fabs(xLidarFil) > threashholds[3]) replace_value(xLidarFil, replace_values[3], replace_counter);
        if(std::fabs(yLidarFil) > threashholds[3]) replace_value(yLidarFil, replace_values[3], replace_counter);
        if(std::fabs(xLidarRaw) > threashholds[3]) replace_value(xLidarRaw, replace_values[3], replace_counter);
        if(std::fabs(yLidarRaw) > threashholds[3]) replace_value(yLidarRaw, replace_values[3], replace_counter);

        line_counter++;

        newline = "";

        //Add Time
        newline += std::to_string(time) + ",";
        //Add Angles
        newline += std::to_string(xAngle) + "," + std::to_string(yAngle) + ",";
        //Add Velocities
        newline += std::to_string(xVelocity) + "," + std::to_string(yVelocity) + ",";
        //Add Positions
        newline += std::to_string(xController) + "," + std::to_string(yController) + ",";
        //Add Force
        newline += std::to_string(xForce) + "," + std::to_string(yForce) + ",";
        //Add Lidar
        newline += std::to_string(xLidarFil) + "," + std::to_string(yLidarFil) + ",";
        newline += std::to_string(xLidarRaw) + "," + std::to_string(yLidarRaw) + "\n";
        outputFile << newline;
    }

    outputFile.close();

    std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - start_time;
	float exectution_time = (ms_double.count()/1000);

    std::cout << "Analyized " << line_counter << " lines" << std::endl;
    std::cout << "Replaced " << replace_counter << " in " << exectution_time << " secounds" << std::endl;

}