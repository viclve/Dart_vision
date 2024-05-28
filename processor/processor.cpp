#include <csignal>
#include <ceres/ceres.h>
#include <Eigen/Core>
#include <Eigen/Dense>
#include "processor.hpp"
#include "camera.hpp"
#include "opencv2/opencv.hpp"
#include "detector.hpp"


using namespace std;
using namespace cv;


// variables
// static volatile bool RUN = true;
// bool show_img = true;

GuideLightDetector GL_detector();

mutex mtx_gl_0, mtx_gl_1, mtx_gl_2;
cv::Point2f gl_0, gl_1, gl_2;

Params param_cam_0, param_cam_1, param_cam_2;

Eigen::Matrix<float, 4, 4> B2CL, B2CM, B2CR;

// ceres solver
ceres::Problem problem;

cv::Point2f Left, Middle, Right;


void ProcessorInit() {
    param_cam_0.loadCalibration("MV-SUA133GC#0");
    param_cam_1.loadCalibration("MV-SUA133GC#1");
    param_cam_2.loadCalibration("MV-SUA133GC#2");

    B2CL << 1, 0, 0, -0.1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;
    B2CM << 1, 0, 0, 0, 0, 1, 0, -0.1, 0, 0, 1, 0, 0, 0, 0, 1;
    B2CR << 1, 0, 0, 0.1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;
}


// // ctrl + c
// void SignalHandler(int signal) {
//     if (signal == SIGINT) {
//         RUN = false;
//     }
// }


double CountTime(void) {
    static timeval current_time;
    static timeval last_time;

    gettimeofday(&current_time, NULL);
    double t = 1000 * (current_time.tv_sec - last_time.tv_sec) + (current_time.tv_usec - last_time.tv_usec) / 1000;
    last_time = current_time;

    // cout<<"dt: "<<t<<endl;
    return t;
}


void PreProcessor(String cam_name) {
    cout<<"start Processor for " + cam_name <<endl;

    Camera* cam = new MVCamera(cam_name);
    GuideLightDetector GL_detector;
    cv::Mat img;

    while (true) {
        // pre process
        bool status = cam->getFrame(img);

        if (cam_name == "MV-SUA133GC#0") {
            if (mtx_gl_0.try_lock()) {
                gl_0 = GL_detector.FindGuideLightinImg(img);
                Left = gl_0;
                mtx_gl_0.unlock();
            }
        } else if (cam_name == "MV-SUA133GC#1") {
            if (mtx_gl_1.try_lock()) {
                gl_1 = GL_detector.FindGuideLightinImg(img);
                Middle = gl_1;
                mtx_gl_1.unlock();
            }
        } else if (cam_name == "MV-SUA133GC#2") {
            if (mtx_gl_2.try_lock()) {
                gl_2 = GL_detector.FindGuideLightinImg(img);
                Right = gl_2;
                mtx_gl_2.unlock();
            }
        }

        // // process ctrl + c
        // signal(SIGINT, SignalHandler);
        // if (!RUN) {
        //     cam->close();
        //     cout<<cam_name + " safely quit"<<endl;
        //     break;
        // }
    }
}

// void GetLine(cv::Point2f point, ) {

// }

void SortGuideLight(cv::Point2f &left, cv::Point2f &middle, cv::Point2f &right) {
    // find left
    if (left.x < middle.x) swap(left, middle);
    if (left.x < right.x) swap(left, right);

    // find middle
    if (right.y > middle.y) swap(right, middle);
}

void LocateGuideLight() {
    static bool cam_0_is_ok = false;
    static bool cam_1_is_ok = false;
    static bool cam_2_is_ok = false;
    static cv::Point2f left, middle, right;

    // get guide light
    // if (mtx_gl_0.try_lock()) {left = gl_0; mtx_gl_0.unlock();}
    // if (mtx_gl_1.try_lock()) {middle = gl_1; mtx_gl_1.unlock();}
    // if (mtx_gl_2.try_lock()) {right = gl_2; mtx_gl_2.unlock();}

    if (!cam_0_is_ok && mtx_gl_0.try_lock()) {left = gl_0; mtx_gl_0.unlock(); cam_0_is_ok = true;}
    if (!cam_1_is_ok && mtx_gl_1.try_lock()) {middle = gl_1; mtx_gl_1.unlock(); cam_1_is_ok = true;}
    if (!cam_2_is_ok && mtx_gl_2.try_lock()) {right = gl_2; mtx_gl_2.unlock(); cam_2_is_ok = true;}

    if (!cam_0_is_ok || !cam_1_is_ok || !cam_2_is_ok) return;

    // sort guide light
    SortGuideLight(left, middle, right);

    // if(left.x == -99999 || left.y == -99999 || middle.x == -99999 || middle.y == -99999 || right.x == -99999 || right.y == -99999){
    //     cout << "got -99999" << endl;
    //     return;
    // }

    // construct function
    Eigen::Vector3f ul, um, ur;
    Eigen::Vector3f pl, pm, pr;

    double baseline = 0.295; 
    double height = 0.27072; //the height of the middle camera 
    int pic_height = 1080;
    int pic_width = 1440;

    pl << baseline / 2, 0      , 0;
    pm << 0           , height , 0;
    pr << -baseline / 2, 0      , 0;

    double cmos_width = 0.000004; //the width and length of mindvision 133 camera are the same

    cout<<"left.x "<<left.x<<" "<<"left.y "<<left.y<<endl;
    cout<<"middle.x "<<middle.x<<" "<<"middle.y "<<middle.y<<endl;
    cout<<"right.x "<<right.x<<" "<<"right.y "<<right.y<<endl;

    ul << (pic_width / 2 - left.x) * cmos_width   , (pic_height / 2 - left.y) * cmos_width   , 0.05;
    um << (pic_width / 2 - middle.x) * cmos_width , (pic_height / 2 - middle.y) * cmos_width , 0.05;
    ur << (pic_width / 2 - right.x) * cmos_width  , (pic_height / 2 - right.y) * cmos_width  , 0.05;
    
    // cout<<"ul[0]"<<ul[0]<<" "<<"ul[1]"<<ul[1]<<endl;
    // cout<<"um[0]"<<um[0]<<" "<<"um[1]"<<um[1]<<endl;
    // cout<<"ur[0]"<<ur[0]<<" "<<"ur[1]"<<ur[1]<<endl;

    Eigen::MatrixXf G = Eigen::MatrixXf::Zero(9, 6);
    Eigen::MatrixXf d = Eigen::MatrixXf::Zero(9, 1);

    for (int i = 0; i < 3; i++) {
        G(i, i) = 1; G(i + 3, i) = 1; G(i + 6, i) = 1;
    }
    G(0, 3) = -ul(0); G(1, 3) = -ul(1); G(2, 3) = -ul(2);
    G(3, 4) = -um(0); G(4, 4) = -um(1); G(5, 4) = -um(2);
    G(6, 5) = -ur(0); G(7, 5) = -ur(1); G(8, 5) = -ur(2);

    d(0, 0) = pl(0); d(1, 0) = pl(1); d(2, 0) = pl(2);
    d(3, 0) = pm(0); d(4, 0) = pm(1); d(5, 0) = pm(2);
    d(6, 0) = pr(0); d(7, 0) = pr(1); d(8, 0) = pr(2);

    Eigen::MatrixXf m = (G.transpose() * G).inverse() * G.transpose() * d;

    // cout << m <<endl;
    cout<<"x: "<<m(0)<<", y: "<<m(1)<<", z: "<<m(2)<<endl;

    cam_0_is_ok = false;
    cam_1_is_ok = false;
    cam_2_is_ok = false;
}

void Process() {
    while (true) {
        // cal fps
        CountTime();

        // main process
        LocateGuideLight();

        // // process ctrl + c
        // signal(SIGINT, SignalHandler);
        // if (!RUN) {
        //     cout<<"processor safely quit"<<endl;
        //     break;
        // }
    }
}

