//c++ includes
#include <chrono>
#include <cmath>
#include <string>
#include <stdio.h>
#include <stdlib.h>

//opencv includes
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>

#define THRESHOLD 10000

const int image_height = 400;
const int image_width = 500;

const cv::Scalar blueLow = cv::Scalar(80, 100, 100); //110 100 100
const cv::Scalar blueHigh = cv::Scalar(125, 255, 255); //125 255 255

cv::Mat BLACK(1000, 1000, CV_64FC4);
cv::Mat BLUE(1000,1000, CV_64FC4);

cv::Mat videoframe;
cv::Mat detectionframe;

double times[3] = {0.0, 0.0, 0.0};

void iteration(int cameraID){
    if(cameraID < 0) throw std::invalid_argument("cameraID cannot be negative");
	std::stringstream path_to_camera;
	path_to_camera << "/dev/video" << cameraID;
    cv::VideoCapture camera(path_to_camera.str());

    camera.set(3, image_width); //could be used to reduce delay :thinking:
	camera.set(4, image_height);

    cv::imshow("LatencyWindow", BLACK);

    while(1){
        camera.read(videoframe);

        cv::cvtColor(videoframe, detectionframe, cv::COLOR_BGR2HSV);
        cv::inRange(detectionframe, blueLow, blueHigh, detectionframe);

        cv::cvtColor(videoframe, detectionframe, cv::COLOR_BGR2HSV);
        cv::inRange(detectionframe, blueLow, blueHigh, detectionframe);

        cv::imshow("VideoFeed", videoframe);
        cv::imshow("Detection", detectionframe);

        //wait for space key
        if(cv::waitKey(1)==32){break; }
    }

    cv::imshow("LatencyWindow", BLUE);

    std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double;

    int camera_retries = 0;
    double total_read_time = 0;
    double total_calc_time = 0;

    while(1){
        std::chrono::_V2::system_clock::time_point start_of_frame = std::chrono::high_resolution_clock::now();
        camera.read(videoframe);   
        ms_double = std::chrono::high_resolution_clock::now() - start_of_frame;   

        total_read_time += ms_double.count();

        camera_retries++;

        std::chrono::_V2::system_clock::time_point start_of_calc = std::chrono::high_resolution_clock::now();

        cv::cvtColor(videoframe, detectionframe, cv::COLOR_BGR2HSV);
        cv::inRange(detectionframe, blueLow, blueHigh, detectionframe);

        cv::erode(detectionframe, detectionframe, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
        cv::dilate(detectionframe, detectionframe, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
        cv::dilate(detectionframe, detectionframe, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
        cv::erode(detectionframe, detectionframe, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));

        std::vector<std::vector<cv::Point>> contours;	
	    findContours(detectionframe, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

        cv::imshow("VideoFeed", videoframe);
        cv::imshow("Detection", detectionframe);

        //wait for space key
        for(int i = 0; i < contours.size(); i++){
            double curArea = cv::contourArea(contours.at(i));
            
            if(curArea > THRESHOLD){
                ms_double = std::chrono::high_resolution_clock::now() - start_of_calc;
                total_calc_time += ms_double.count();

                ms_double = std::chrono::high_resolution_clock::now() - start;

                times[0] = ms_double.count();
                times[1] = total_read_time;
                times[2] = total_calc_time;

                std::cout << ms_double.count() << "ms after " << camera_retries << " frames" << std::endl;
                //std::cout << "Total reading time is " << total_time << "ms" << std::endl;
                //std::cout << "Total calc time is " << total_calc_time << "ms" << std::endl;

                return;
            }
	    }

        if(cv::waitKey(1)==32){break; }

        ms_double = std::chrono::high_resolution_clock::now() - start_of_calc;
        total_calc_time += ms_double.count();
    }
}

double calculate_average(double values[], int length){
    double result = 0;

    for(int i = 0; i < length; i++) result += values[i];

    return result / length;
}

double calculate_std_error(double values[], int length){
    double average = calculate_average(values, length);
    double sum = 0;

    for(int i = 0; i < length; i++) sum += pow(values[i] - average,2);

    return sqrt(sum / (length - 1))/sqrt(length);
}

void help(){
    std::cout << "Use this programm to find the latency of any camera" << std::endl;
    std::cout << "Command Usage: ./find-latency <Cam_ID> <Iterations>" << std::endl;
    std::cout << std::endl << "How to use:" << std::endl;
    std::cout << "The program will open 3 windows. One showing the video stream, one showing all blue pixels in the video and one big window showing a black screen" << std::endl;
    std::cout << "Point your camera at the big black window. Once you're satisfied with what the camera sees press space." << std::endl;
    std::cout << "The screen will flash blue and calculate the time it takes for the camera to register it." << std::endl;
    std::cout << "Repeat this process <Iterations> times." << std::endl;
    exit(0);
}

int main(int argc, char** argv){

    if(strcmp(argv[1], "-h") == 0) help();

    //init
    BLACK = cv::Scalar(0, 0, 0);
    BLUE = cv::Scalar(255, 0, 0);

    cv::namedWindow("LatencyWindow");
    cv::namedWindow("VideoFeed");
    cv::namedWindow("Detection");

    int cameraID = strtol(argv[1], NULL, 10);
    int iterations = strtol(argv[2], NULL, 10);

    double all_times[iterations], all_read_times[iterations], all_calc_times[iterations];

    for(int i = 0; i < iterations; i++){
        std::cout << "----------------------------" << std::endl;
        std::cout << "Iteration " << i << std::endl;

        iteration(cameraID);

        all_times[i] = times[0];
        all_read_times[i] = times[1];
        all_calc_times[i] = times[2];
    }

    std::cout << "----------------------------" << std::endl;
    std::cout << "Total latency is " << calculate_average(all_times, iterations) << " +- " << calculate_std_error(all_times, iterations) << "ms" << std::endl;
    std::cout << "Total reading time is " << calculate_average(all_read_times, iterations) << " +- " << calculate_std_error(all_read_times, iterations) << "ms" << std::endl;
    std::cout << "Total calc time is " << calculate_average(all_calc_times, iterations) << " +- " << calculate_std_error(all_calc_times, iterations) << "ms" << std::endl;
    
}