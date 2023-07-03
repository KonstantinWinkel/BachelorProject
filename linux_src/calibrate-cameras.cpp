//c++ includes
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <fstream>

//opencv includes
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//own includes
#include "ImageProcessing.h"


float calibrate(int camera_id, int values){

    ImageProcessing current(camera_id, "lmao");

    //spacer
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "Starting calibration of camera" << camera_id << std::endl;
    std::cout << "Please place the camera so that it can fully see the axis it will be used for." << std::endl;
    std::cout << "Once youre done press 'SPACEBAR' to start the calibration." << std::endl; 

    current.showVideoStream();
    
    float sum_of_angles = 0;
    float current_angle = 0;

    std::chrono::_V2::system_clock::time_point start_time = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < values; i++){
        current_angle = current.processOneImage();
        sum_of_angles += current_angle;

        std::cout << "Value " << i << ": " << current_angle << std::endl;
    }

    float result = sum_of_angles / values - 90.0;

    std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - start_time;

    std::cout << "Calibration value: " << result <<" - calutlated in " << (ms_double.count()/1000.0) << " secounds" << std::endl;

    return result;
}

int main(int argc, char** argv){

    //calibrate both cameras
    float x_calib_value = calibrate(strtol(argv[1], NULL, 10), strtol(argv[3], NULL, 10));
    float y_calib_value = calibrate(strtol(argv[2], NULL, 10), strtol(argv[3], NULL, 10));

    std::fstream output_file;

    //init file
    output_file.open("camera.calib", std::ofstream::out | std::ofstream::trunc);

    if(!output_file){
        output_file.open("camera.calib", std::fstream::in | std::fstream::out | std::fstream::trunc);
        output_file << "\n";
        output_file.close();
    }
    else output_file.close();

    //write values to file
    output_file.open("camera.calib");
    output_file << x_calib_value << "\n" << y_calib_value << "\n";
    output_file.close();

    return 0;
}