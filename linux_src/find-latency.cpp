//c++ includes
#include <chrono>
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

int main(int argc, char** argv){

    cv::Mat BLACK(1000, 1000, CV_64FC4);
    cv::Mat BLUE(1000,1000, CV_64FC4);
    cv::Mat videoframe;
    cv::Mat detectionframe;

    BLACK = cv::Scalar(0, 0, 0);
    BLUE = cv::Scalar(255, 0, 0);

    cv::namedWindow("LatencyWindow");
    cv::namedWindow("VideoFeed");
    cv::namedWindow("Detection");


    int cameraID = strtol(argv[1], NULL, 10);
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
    double total_time = 0;
    double total_calc_time = 0;

    while(1){
        std::chrono::_V2::system_clock::time_point start_of_frame = std::chrono::high_resolution_clock::now();
        camera.read(videoframe);   
        ms_double = std::chrono::high_resolution_clock::now() - start_of_frame;   

        total_time += ms_double.count();

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
                std::cout << "Total latency is " << ms_double.count() << "ms after " << camera_retries << " frames" << std::endl;
                std::cout << "Total reading time is " << total_time << "ms" << std::endl;
                std::cout << "Total calc time is " << total_calc_time << "ms" << std::endl;

                return 0;
            }
	    }

        if(cv::waitKey(1)==32){break; }

        ms_double = std::chrono::high_resolution_clock::now() - start_of_calc;
        total_calc_time += ms_double.count();
    }

}