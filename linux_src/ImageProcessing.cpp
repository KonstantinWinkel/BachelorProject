//c++ includes
#include <stdio.h>
#include <cmath>

//opencv includes
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//class header
#include "ImageProcessing.h"

const cv::Scalar blueLow = cv::Scalar(80, 100, 100); //110 100 100
const cv::Scalar blueHigh = cv::Scalar(125, 255, 255); //125 255 255

//constructor, creates ImageProcessing object, sets identifying variables and intializes OpenCV Camera
ImageProcessing::ImageProcessing(int cameraID, std::string name, FileWriter * filewriter, Identifier angleIdentifier, Identifier velocityIdentifier){
	ImageProcessing::cameraID = cameraID;
	ImageProcessing::name = name;
	ImageProcessing::filewriter = filewriter;
	ImageProcessing::angleIdentifier = angleIdentifier;
	ImageProcessing::velocityIdentifier = velocityIdentifier;

	videoWindowName = "V" + name;
	detectionWindowName = "D" + name;

	cv::VideoCapture localCamera(cameraID);
	camera = localCamera;

	cv::namedWindow(videoWindowName);
	cv::namedWindow(detectionWindowName);

	firstIteration = true;
}

//destructor, left empty
ImageProcessing::~ImageProcessing(){
	
}

//calculates the angular position and velocity from processed image
void ImageProcessing::CalculatePositionAndVelocity(){
	
	cv::Point angleComputation;
	angleComputation.x = points.at(1).x - points.at(0).x;
	angleComputation.y = points.at(1).y - points.at(0).y;

	cv::line(videoFrame, points.at(0), points.at(1), cv::Scalar(255,255,255), 2, cv::LINE_8);
	double tempangle = -atan2(angleComputation.y, angleComputation.x) * 180/M_PI;

	std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - lastIteration;

	if(!firstIteration) velocity = (tempangle - angle)/(ms_double.count()/1000);

	firstIteration = false;
	lastIteration = std::chrono::high_resolution_clock::now();
	angle = tempangle;

	std::cout << angle << " " << velocity << std::endl;
}

//saves a frame from the camera and processes it --> finding coloured contrours
void ImageProcessing::ReadAndProcessImage(){

	bool successfullRead = camera.read(videoFrame);

	cv::cvtColor(videoFrame, detectionFrame, cv::COLOR_BGR2HSV);
	cv::inRange(detectionFrame, blueLow, blueHigh, detectionFrame);


	//dont delete just yet, might be important
	cv::erode(detectionFrame, detectionFrame, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
	cv::dilate(detectionFrame, detectionFrame, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
	cv::dilate(detectionFrame, detectionFrame, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
	cv::erode(detectionFrame, detectionFrame, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));

	std::vector<std::vector<cv::Point>> contours;	
	findContours(detectionFrame, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	//if the size of contours is smaller than 2, its not possible to construct a line
	if(contours.size() < 2){
		printf("skipped frame \n");
		skipFrame = true;
		return;
	}

	//find 2 largest blobs
	int aID = 0, bID = 1; //a = largest, b = secound largest
	double aMaxArea = 0, bMaxArea = 0;

	for(int i = 0; i < contours.size(); i++){
		double curArea = cv::contourArea(contours.at(i));
		
		if(curArea > aMaxArea){
			bID = aID; aID = i;
			bMaxArea = aMaxArea; aMaxArea = curArea;
			continue;
		}

		if(curArea > bMaxArea){
			bID = i;
			bMaxArea = curArea;
		}
	}

	//find the middle points of the 2 largest blobs
	std::vector<cv::Point> largestPoints;

	cv::Rect aRect = cv::boundingRect(contours.at(aID));
	cv::Point a(aRect.x+aRect.width/2, aRect.y+aRect.height/2);

	cv::Rect bRect = cv::boundingRect(contours.at(bID));
	cv::Point b(bRect.x+bRect.width/2, bRect.y+bRect.height/2);

	//ensures that the first point of th vector is the highest
	if(a.y > b.y){
		largestPoints.push_back(a);
		largestPoints.push_back(b);
	}
	else{
		largestPoints.push_back(b);
		largestPoints.push_back(a);
	}

	points = largestPoints;
}

void ImageProcessing::PublishValues(){
	filewriter->writeFLOAT(angleIdentifier, angle);
	filewriter->writeFLOAT(velocityIdentifier, velocity);
}

//run method, handles programm flow
void ImageProcessing::run(){
	while(1){
		skipFrame = false;
		ReadAndProcessImage();
		if(!skipFrame){
			CalculatePositionAndVelocity();
			PublishValues();
		}

		cv::imshow(videoWindowName, videoFrame);
		cv::imshow(detectionWindowName, detectionFrame);

		if(cv::waitKey(1)==1){return; }
	}
}