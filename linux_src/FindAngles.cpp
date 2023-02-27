//c++ things
#include <stdio.h>
#include <ctime> //only for testing, can be removed one finished
#include <cmath>
#include <thread>

//opencv things
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

std::clock_t start;

Mat actual_Image;

double yvals[2]; //0 = yangle, 1 = ydot
double xvals[2]; //0 = xangle, 1 = xdot
std::clock_t lastIteration[2]; //0 = x, 1 = y
std::clock_t cycle;


void calculatePositionAndVelocity(std::vector<cv::Point> points, char XorY, Mat abc){

	cv::Point high, right, left;
	high = points.at(0);
	right = points.at(0);
	left = Point(1000,1000);

	for(int i = 1; i < points.size(); i++){
		if(points.at(i).y < high.y) high = points.at(i);
		if(points.at(i).x > right.x) right = points.at(i);
		if(points.at(i).x < left.x) left = points.at(i);
	}
	
	if(left.y < right.y){
		right.x = high.x;
		right.y = high.y;
		high.x = left.x;
		high.y = left.y;
	}
	
	cv::Point angleComputation;
	angleComputation.x = right.x - high.x;
	angleComputation.y = -(right.y - high.y);

	double angle = atan2(angleComputation.y, angleComputation.x) * 180/M_PI;

	if(XorY == 'x'){
		double timediff = (float)(clock() - lastIteration[0])/CLOCKS_PER_SEC;
		lastIteration[0] = clock();
		xvals[1] = (angle-xvals[0])/timediff;
		xvals[0] = angle;
		
		circle(abc, Point(high.x, high.y), 200/32, Scalar(0, 0, 255), FILLED, LINE_8);
		circle(abc, Point(right.x, right.y), 200/32, Scalar(0, 255, 0), FILLED, LINE_8);
		circle(abc, Point(left.x, left.y), 200/32, Scalar(255, 0, 0), FILLED, LINE_8);
		
		printf("Points:\n");
		printf("High: %d, %d\n", high.x, high.y);
		printf("Right: %d, %d\n", right.x, right.y);
		printf("Angle: %f\n", xvals[0]);
		printf("Speed: %f\n", xvals[1]);
		 
		printf("Cycle: %f\n\n", (float)(clock() - cycle)/CLOCKS_PER_SEC);
		return;
	}

	if(XorY == 'y'){
		double timediff = (float)(clock() - lastIteration[1])/CLOCKS_PER_SEC;
		lastIteration[1] = clock();
		yvals[1] = (angle-yvals[0])/timediff;
		yvals[0] = angle;
		return;
	}
}

void Example(){
	VideoCapture video_load(0);
	
	Scalar blueLow = Scalar(110, 100, 100);
	Scalar blueHigh = Scalar(125, 255, 255);

	int Horizontal_Last = -1;
	int Vertical_Last = -1;
	Mat temp;
	video_load.read(temp);
	Mat track_motion = Mat::zeros(temp.size(), CV_8UC3);
	while(true){
		cycle = clock();
		bool temp_load = video_load.read(actual_Image);
		Mat converted_to_HSV;
		cvtColor(actual_Image, converted_to_HSV, COLOR_BGR2HSV);
		Mat adjusted_frame;
		inRange(converted_to_HSV, blueLow, blueHigh, adjusted_frame);
			
		//dont delete just yet, might be important
		erode(adjusted_frame, adjusted_frame, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
		dilate(adjusted_frame, adjusted_frame, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
		dilate(adjusted_frame, adjusted_frame, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
		erode(adjusted_frame, adjusted_frame, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

		std::vector<std::vector<cv::Point>> contours;
		
		findContours(adjusted_frame, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
		
		int maxID = -1;
		double maxArea = 0;	
		for(int i = 0; i < contours.size(); ++i)
		{
			double curArea = cv::contourArea(contours.at(i));
			if(curArea > maxArea){
				maxArea = curArea;
				maxID = i;
			}
		}

		printf("%d aus %d\n", maxID, contours.size());
		

		if(maxID != -1)
		{
			//mabye remove for PERFORMANCE
			//printf("Größe vor: %d\n", contours[maxID].size());
			//double len = cv::arcLength(contours[maxID], true); 
			//cv::approxPolyDP(contours[maxID], contours[maxID], 0, true);
			//printf("Größe nach: %d\n", contours[maxID].size());
			calculatePositionAndVelocity(contours[maxID], 'x', actual_Image);
		}

		imshow("Detected_Object", adjusted_frame);
		imshow("Actual", actual_Image);
		
		if(waitKey(100)==1){ break; }
	}
}


int main(int argc, char** argv)
{
	/*
	//decleration of vars
	Mat imageCamera1;
	namedWindow("Camera 1");
	VideoCapture cam1(0);
	
	Mat imageCamera2;
	namedWindow("Camera 2");
	VideoCapture cam2(0);
	lower_blue = Scalar(110, 100, 100);
	upper_blue = Scalar(125, 255, 255);
	
	if(!cam1.isOpened()) printf("cannot open camera 1");
	if(!cam2.isOpened()) printf("cannot open camera 2");
	
	while(true){
		start = clock();
		
		cam1 >> imageCamera1;
		Mat detection;
		
		imshow("Camera 1", FindAngle(imageCamera1, detection));
		
		
		//cam2 >> imageCamera2;
		//imshow("Camera 2", imageCamera2);
		
		printf("%fs\n", (float)(clock() - start)/CLOCKS_PER_SEC);
		
		waitKey(25); //was macht das?
		
		
	}	
	
	return 0;
	
	*/
	
	Example();

	//std::thread threadOne(Example);
	//threadOne.join();

	return 0;	
}
