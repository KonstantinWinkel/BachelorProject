//c++ includes
#include <stdio.h>
#include <cmath>
#include <fstream>

//opencv includes
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//class header
#include "ImageProcessing.h"

const cv::Scalar blueLow = cv::Scalar(80, 100, 100); //110 100 100
const cv::Scalar blueHigh = cv::Scalar(125, 255, 255); //125 255 255

const int image_height = 400;
const int image_width = 500;

//                          w  h  w    h
const int RECT_PARAMS[4] = {200, 100, 299, 260}; //400x500 -> {}

//constructor, creates ImageProcessing object, sets identifying variables and intializes OpenCV Camera
ImageProcessing::ImageProcessing(int cameraID, std::string name, FileWriter * filewriter, Controller * controller, Identifier identifier){
	ImageProcessing::cameraID = cameraID;
	ImageProcessing::name = name;
	ImageProcessing::filewriter = filewriter;
	ImageProcessing::controller = controller;
	ImageProcessing::identifier = identifier;

	videoWindowName = "V" + name;
	detectionWindowName = "D" + name;

	if(cameraID < 0) throw std::invalid_argument("cameraID cannot be negative");
	std::stringstream path_to_camera;
	path_to_camera << "/dev/video" << cameraID;

	std::cout << "Trying to access camera " << cameraID << "(path " << path_to_camera.str() <<')' << std::endl;
	cv::VideoCapture localCamera(path_to_camera.str());
	camera = localCamera;

	ROI = cv::Rect(RECT_PARAMS[0], RECT_PARAMS[1], RECT_PARAMS[2], RECT_PARAMS[3]);

	camera.set(3, image_width); //could be used to reduce delay :thinking:
	camera.set(4, image_height);

	/*cv::namedWindow(videoWindowName);
	cv::namedWindow(detectionWindowName);

	cv::moveWindow(videoWindowName, 0, 0);
	cv::moveWindow(detectionWindowName, 0, 500);

	if(identifier == Identifier::Y){
		cv::moveWindow(videoWindowName, 500, 0);
		cv::moveWindow(detectionWindowName, 500, 500);
	}
*/

	firstIteration = true;

	std::ifstream calib_file("camera.calib");

	if(calib_file){
		double values[2];

		calib_file >> values[0];
		calib_file >> values[1];	

		if(ImageProcessing::identifier == Identifier::X) calib_correction = values[0];
		if(ImageProcessing::identifier == Identifier::Y) calib_correction = values[1];
	}

}

ImageProcessing::ImageProcessing(int cameraID, std::string name){
	ImageProcessing::cameraID = cameraID;
	ImageProcessing::name = name;

	videoWindowName = "V" + name;
	detectionWindowName = "D" + name;

	if(cameraID < 0) throw std::invalid_argument("cameraID cannot be negative");
	std::stringstream path_to_camera;
	path_to_camera << "/dev/video" << cameraID;

	std::cout << "Trying to access camera " << cameraID << "(path " << path_to_camera.str() <<')' << std::endl;
	cv::VideoCapture localCamera(path_to_camera.str());
	camera = localCamera;

	ROI = cv::Rect(RECT_PARAMS[0], RECT_PARAMS[1], RECT_PARAMS[2], RECT_PARAMS[3]);

	camera.set(3, image_width); //could be used to reduce delay :thinking:
	camera.set(4, image_height);

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

	double tempangle = -atan2(angleComputation.y, angleComputation.x) - calib_correction;

	std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - lastIteration;

	if(!firstIteration) velocity = (tempangle - angle)/(ms_double.count()/1000);

	firstIteration = false;
	lastIteration = std::chrono::high_resolution_clock::now();
	angle = tempangle;

	_debug_print_image_(angle << " " << velocity);
}

//saves a frame from the camera and processes it --> finding coloured contrours
void ImageProcessing::ReadAndProcessImage(){

	bool successfullRead = camera.read(videoFrame);

	videoFrame = videoFrame(ROI);

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
		_debug_print_image_("skipped frame");
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

	cv::line(videoFrame, points.at(0), points.at(1), cv::Scalar(255,255,255), 2, cv::LINE_8);
}

void ImageProcessing::PublishValues(){
	filewriter->writeCameraInfo(identifier, angle, velocity);
	controller->recieve_angle(angle);
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

		//maybe remove?
		if(cv::waitKey(1)==1){return; }
	}
}

void ImageProcessing::showVideoStream(){
	while(1){
		ReadAndProcessImage();

		cv::imshow(videoWindowName, videoFrame);
		cv::imshow(detectionWindowName, detectionFrame);

		//wait for space key
		if(cv::waitKey(1)==32){break; }
	}

	cv::destroyAllWindows();
}

float ImageProcessing::processOneImage(){
	skipFrame = false;
	ReadAndProcessImage();
	if(!skipFrame){
		CalculatePositionAndVelocity();
	}

	return angle;
}