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

class ImageProcessing{
    protected:
        FileWriter * filewriter;
        Identifier angleIdentifier;
        Identifier velocityIdentifier;

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
        ImageProcessing(int cameraID, std::string name, FileWriter * filewriter, Identifier angleIdentifier, Identifier velocityIdentifier);
        ~ImageProcessing();
        virtual void ReadAndProcessImage();
        virtual void CalculatePositionAndVelocity();
        virtual void PublishValues();

        virtual void run();

};

#endif