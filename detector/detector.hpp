#ifndef __DETECTOR_HPP__
#define __DETECTOR_HPP__

#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


using namespace cv;
using namespace std;


class GuideLightDetector {

public:
    GuideLightDetector(){
        this->green_lower = cv::Scalar(30, 0, 50);
        this->green_upper = cv::Scalar(90, 255, 255);
        // this->green_lower = cv::Scalar(0, 0, 120);
        // this->green_upper = cv::Scalar(120, 255, 255);

        this->min_area = 550;
        // this->max_area = 1200;
        this->max_area = 1500;      
        this->wh_ratio = 1.1;

        // cv::FileStorage fConfig("calibration", cv::FileStorage::READ);
        // if (!fConfig.isOpened()) {
        //     std::cout << "Open config File:" << "calibration" << " failed."<<endl;
        //     return;
        // }
    }

    cv::Point3f FindGuideLightinImg(cv::Mat &img);

private:
    cv::Point2f gl_in_img;
    cv::Scalar green_lower;
    cv::Scalar green_upper;
    double min_area;
    double max_area;
    double wh_ratio;

    bool IsGuideLight();
};


class DartDetector {

public:
    DartDetector(){
        this->red_lower = cv::Scalar(0, 0, 0);
        this->red_upper = cv::Scalar(255, 255, 255);
        this->blue_lower = cv::Scalar(0, 0, 0);
        this->blue_upper = cv::Scalar(255, 255, 255);
    }

private:
    cv::Point3d dart;

    cv::Scalar red_lower;
    cv::Scalar red_upper;
    cv::Scalar blue_lower;
    cv::Scalar blue_upper;
};

#endif
