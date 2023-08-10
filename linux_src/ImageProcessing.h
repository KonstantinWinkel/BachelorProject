#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

//c++ includes
#include <chrono>

//opencv includes
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>

//required header files
#include "FileWriter.h"
#include "Filter.h"

#if defined(_DEBUG_IMAGE_ON_)
#define _debug_print_image_(x) std::cout << x << std::endl
#else
#define _debug_print_image_(x)
#endif

#include "bachelor_debug.h"

#if defined(_DEBUG_IMAGE_ON_)
#define _debug_print_image_(x) std::cout << x << std::endl
#else
#define _debug_print_image_(x)
#endif

using namespace je;

class ImageProcessing{
	protected:
		FileWriter * filewriter;
		Filter * filter;
		Identifier identifier;

		int cameraID;
		std::string name;

		bool firstIteration;
		bool skipFrame;

		std::vector<cv::Point> points;
		double angle;
		double velocity;
		double calib_correction;
		std::chrono::_V2::system_clock::time_point lastIteration;

		cv::VideoCapture camera;
		cv::Mat videoFrame;
		cv::Mat detectionFrame;

		cv::Rect ROI; //600 x 480

		std::string videoWindowName;
		std::string detectionWindowName;

	public:
		ImageProcessing();
		ImageProcessing(int cameraID, std::string name);
		ImageProcessing(int cameraID, std::string name, FileWriter * filewriter, Filter * filter, Identifier identifier);
		~ImageProcessing();
		virtual void ReadAndProcessImage();
		virtual void CalculatePositionAndVelocity();
		virtual void PublishValues();

		virtual void run();

		virtual void showVideoStream();
		virtual float processOneImage();

};

#endif