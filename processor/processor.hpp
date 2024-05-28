#ifndef __PROCESSOR_HPP_
#define __PROCESSOR_HPP_

#include <iostream>
#include <sys/time.h>
#include <mutex>
#include <thread>  
#include <queue>  
#include <mutex>  
#include <condition_variable> 

#include "opencv2/opencv.hpp"
#include "camera.hpp"
#include "detector.hpp"
#include "params.hpp"


using namespace std;
using namespace cv;


double CountTime(void);

void PreProcessor(String cam_name);

void Process();

void ProcessorInit();

#endif