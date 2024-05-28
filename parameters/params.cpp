#include "params.hpp"

void Params::loadCalibration(const string &calibration){
    cv::FileStorage fConfig(calibration,cv::FileStorage::READ);
    if (!fConfig.isOpened()) {
        std::cout << "Open config File:" << calibration << " failed."<<endl;
        return;
    }
    camera_width = fConfig["camera_width"];
    camera_height = fConfig["camera_height"];
    CameraMatrix.at<double>(0, 0) = fConfig["CameraMatrix_00"];
	CameraMatrix.at<double>(1, 1) = fConfig["CameraMatrix_11"];
	CameraMatrix.at<double>(0, 2) = fConfig["CameraMatrix_02"];
	CameraMatrix.at<double>(1, 2) = fConfig["CameraMatrix_12"];
	CameraMatrix.at<double>(2, 2) = 1.0;
    DistCoeffs.at<double>(0, 0) = fConfig["DistCoeffs_00"];
    DistCoeffs.at<double>(0, 1) = fConfig["DistCoeffs_01"];
    DistCoeffs.at<double>(0, 2) = fConfig["DistCoeffs_02"];
    DistCoeffs.at<double>(0, 3) = fConfig["DistCoeffs_03"];
    fConfig["exposure"]   >> exposure;
    fConfig["rGain"]      >> rGain;
    fConfig["gGain"]      >> gGain;
    fConfig["bGain"]      >> bGain;
    fConfig["analogGain"] >> analogGain;
    fConfig["gamma"]      >> gamma;
    fConfig["contrast"]   >> contrast;
    fConfig["brightness"] >> brightness;
    fConfig["Mode"]       >> Mode;
    fConfig["Auto"]       >> Auto;
    fConfig["size_min"]       >> size_min;
    fConfig["size_max"]       >> size_max;
    fConfig["color_cnt"]       >> color_cnt;
    cout<<"load calibration done"<<endl;
}

void Double_Params::loadDoubleParams(const string &config){
    cv::FileStorage fConfig(config, cv::FileStorage::READ);
    R.at<double>(0, 0) = fConfig["R_00"];
    R.at<double>(0, 1) = fConfig["R_01"];
    R.at<double>(0, 2) = fConfig["R_02"];
    R.at<double>(1, 0) = fConfig["R_10"];
    R.at<double>(1, 1) = fConfig["R_11"];
    R.at<double>(1, 2) = fConfig["R_12"];
    R.at<double>(2, 0) = fConfig["R_20"];
    R.at<double>(2, 1) = fConfig["R_21"];
    R.at<double>(2, 2) = fConfig["R_22"];
    T.at<double>(0, 0) = fConfig["T_0"];
    T.at<double>(0, 1) = fConfig["T_1"];
    T.at<double>(0, 2) = fConfig["T_2"];
    B = fConfig["B"];
    pair_threshold = fConfig["pair_threshold"];
    size_threshold = fConfig["size_threshold"];
    cout<<"load double params done"<<endl;
}

void Mode::loadDoubleParams(const string &config) {
    cv::FileStorage fConfig(config, cv::FileStorage::READ);
    fConfig["target_color"]    >> target_color;
    fConfig["debug"]           >> debug;
    fConfig["camera_type"]     >> camera_type;
    fConfig["video_path"]      >> video_path;
    fConfig["enable_serial"]   >> enable_serial;
    fConfig["show_source_img"] >> show_source_img;
    fConfig["show_target_img"] >> show_target_img;
    fConfig["save_video"]      >> save_video;
    cout<<"Load Mode parameters done"<<endl;
}
