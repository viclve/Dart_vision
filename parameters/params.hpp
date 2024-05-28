#ifndef _PARAMS_
#define _PARAMS_

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;


struct Params{
    void loadCalibration(const string &calibration);
    int camera_width, camera_height;
    cv::Mat CameraMatrix = cv::Mat::zeros(Size(3, 3), CV_64F);
    cv::Mat DistCoeffs = cv::Mat::zeros(Size(1, 5), CV_64F);
    /* MVCamera */
    int exposure;
    int rGain, gGain, bGain;
    int analogGain;
    int gamma;
    int contrast;
    int brightness;
    int Mode;
    int Auto;
    double size_min,size_max;
    int color_cnt;
};

struct Double_Params{//从右到左
    void loadDoubleParams(const string &config);
    cv::Mat R = cv::Mat::zeros(Size(3, 3), CV_64F), T = cv::Mat::zeros(Size(1, 3), CV_64F);
    cv::Mat R_left = cv::Mat::zeros(Size(3, 3), CV_64F);
    cv::Mat R_right = cv::Mat::zeros(Size(3, 3), CV_64F);
    cv::Mat P_left = cv::Mat::zeros(Size(3, 3), CV_64F);
    cv::Mat P_right = cv::Mat::zeros(Size(3, 3), CV_64F);
    cv::Mat Q;//深度差异映射矩阵，目前我还没搜到里面的每个参数代表什么
    cv::Mat mapLx, mapLy, mapRx, mapRy;//或许可用的第一种方法，但是目前没用
    int pair_threshold;
    double B;//基轴长度
    double size_threshold; //左右两个灯之间的面积差距不能太荒谬
};

struct Mode{
    void loadDoubleParams(const string &config); 
    int target_color;
    int debug;
    int camera_type;
    int enable_serial;
    int show_source_img;
    int show_target_img;
    int save_video;
    string video_path;
};

#endif