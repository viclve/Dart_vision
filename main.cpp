#include "opencv2/opencv.hpp"
#include <iostream>
#include <csignal>
#include <thread>
#include <mutex>
#include <string>
#include <ctime>

#include "camera.hpp"
#include "params.hpp"
#include "detector.hpp"
#include "SerialCommunicator.hpp"

using namespace cv;
using namespace std;


static volatile bool RUN = true;

void SignalHandler(int signal) {
    if (signal == SIGINT) {
        RUN = false;
    }
}

int main() {
    // single camera
    String cam_name = "MV-SUA133GC#0";
    Mode mode;
    Camera* cam = new MVCamera(cam_name);
    SerialCommunicator comm(true);
    GuideLightDetector GL_detector;
    cv::Mat img;
    cv::VideoCapture cap;  
    cv::VideoWriter rec;

    // load mode
    mode.loadDoubleParams("/home/helloworld/szy/single_cam/parameters/mode.yaml");

    if (mode.camera_type == 1) {
        cap.open(mode.video_path);
        if (!cap.isOpened()) {
            cout<<"Can not find video in the given path: " + mode.video_path<<endl;
            return 0;
        }
    }  
    if (mode.camera_type == 0 && mode.save_video) {
        rec.open(mode.video_path + to_string(time(0)) + ".avi", CV_FOURCC('D', 'I', 'V', 'X'), 30.0, Size(1280, 1024));
    }

    // main loop
    while (true) {
        if (mode.camera_type == 0) bool status = cam->getFrame(img);  // get frame from MVCamera
        else if (mode.camera_type == 1) {
            cap >> img;
            if (img.empty()) {
                cout<<"finished the video!"<<endl;
                break;
            }
        }

        cv::Point3f gl = GL_detector.FindGuideLightinImg(img);

        float dx, dy, area;
        // int first_catch = 0;
        dx = gl.x - 640; dy = gl.y - 512; area = gl.z;
        // if (mode.enable_serial) comm.send({dx, dy,1, 1, 1});   
        if (mode.enable_serial) comm.send({dx, dy, 1, (uint8_t)(area > 1 ? 1 : 0), 1});
        // if (mode.enable_serial) {
        //     if (first_catch != 0) {
        //         if (first_catch = 1 ){
        //             comm.send({1, 1, 1, 1, 1});
        //             first_catch = 2;
        //         } else {
        //             comm.send({dx, dy, 1, 1, 1});
        //         }   
        //     } else {
        //         comm.send({0, 0, 1, 1, 1});
        //         if (dx != 0 & dy != 0) first_catch = 0;
        //     }
        // }

        if (mode.debug) {
            std::string debug_info;
            debug_info = "guide light position: [" + to_string(dx) + ", " + to_string(dy) + "], area: " + to_string(area);
            cv::putText(img, debug_info, Point(50, 1000), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 4, 8);
            cv::imshow("image", img);
            // terminal
            cout << "detect: " << gl << endl;
            cv::waitKey(1);
        }

        if (mode.camera_type == 0 && mode.save_video) {
            rec.write(img);
        }

        // if ((gl.x - 640) > 327){
        //     comm.send({327, gl.y - 512, 1, 1, 1});
        // } else {
        //     if ((gl.x - 640) < -327){
        //         comm.send({-327, gl.y-512, 1, 1, 1});
        //     } else {
        //         comm.send({gl.x-640, gl.y-512, 1, 1, 1});
        //     }   
        // }

        // process ctrl + c
        signal(SIGINT, SignalHandler);
        if (!RUN) {
            if (mode.camera_type == 0) cam->close();
            else if (mode.camera_type == 1) cap.release();
            cout<<cam_name + " safely quit"<<endl;
            break;
        }
    }
}
