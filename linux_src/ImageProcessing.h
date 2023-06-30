#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

//c++ includes
#include <chrono>

//opencv includes
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

//required header files
#include "FileWriter.h"
#include "DataPuffer.h"

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
		CommBuffer<double> * ComBuf;
		Identifier identifier;

		int cameraID;
		std::string name;

		bool firstIteration;
		bool skipFrame;

		std::vector<cv::Point> points;
		double angle;
		double velocity;
		std::chrono::_V2::system_clock::time_point lastIteration;

		cv::VideoCapture camera;
		cv::Mat videoFrame;
		cv::Mat detectionFrame;

		std::string videoWindowName;
		std::string detectionWindowName;

	public:
		ImageProcessing();
		ImageProcessing(int cameraID, std::string name, FileWriter * filewriter, CommBuffer<double> * ComBuf, Identifier identifier);
		~ImageProcessing();
		virtual void ReadAndProcessImage();
		virtual void CalculatePositionAndVelocity();
		virtual void PublishValues();

		virtual void run();

};

#endif