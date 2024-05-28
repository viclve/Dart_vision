#include "detector.hpp"

using namespace std;
using namespace cv;


cv::Point3f GuideLightDetector::FindGuideLightinImg(cv::Mat &img){
    cv::Mat hsv, target_mask;

    if (!img.empty()) cv::imwrite("./img.jpg", img);

    cv::cvtColor(img, hsv, COLOR_BGR2HSV);
    cv::inRange(hsv, green_lower, green_upper, target_mask);

    // 对二值化后的图像进行形态学处理，去除噪点
    cv::Mat kernel = getStructuringElement(MORPH_RECT, Size(10, 10));
    cv::morphologyEx(target_mask, target_mask, MORPH_OPEN, kernel);
    cv::morphologyEx(target_mask, target_mask, MORPH_CLOSE, kernel);
    cv::GaussianBlur(target_mask, target_mask, Size(3, 3), 0);//高斯滤波
    cv::Canny(target_mask, target_mask, 100, 250);//canny算子边缘检测

    // 在二值化后的图像中寻找轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    cv::findContours(target_mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // cv::Mat img_showcontour = img;
    // cv::drawContours(img_showcontour, contours, -1,(0,0,255), 2);

    // cv::imshow("targ",img);
    // cv::imshow("hsv",hsv);
    
    // cv::waitKey(0);
    // cout << "in detector" << endl;

    vector<double> contour_area(contours.size());
    vector<cv::RotatedRect> bound_rect(contours.size());
    vector<int> posibile_guide_light;
    double max_contour_area = 0;
    int max_contour_idx = 0;

    for (int contour_idx = 0; contour_idx < contours.size(); contour_idx++) {
        // cal area
        contour_area[contour_idx] = cv::contourArea(contours[contour_idx]);
        // if(contour_area[contour_idx] > 200){
        //     cout<<"area:"<<contour_area[contour_idx]<<endl;
        // }
        if (contour_area[contour_idx] <= min_area || contour_area[contour_idx] >= max_area) {
            // area out of range
            // cout<<contour_area[contour_idx]<<endl;
            //cout<<"area out of range"<<endl;
            continue;
        }

        // cal wh_ratio
        bound_rect[contour_idx] = cv::minAreaRect(cv::Mat(contours[contour_idx]));
        double ratio = bound_rect[contour_idx].size.width / bound_rect[contour_idx].size.height;
        if (ratio >= wh_ratio || 1 / ratio >= wh_ratio) {
            // wh_ratio out of range
            continue;
        }

        // posibile guide light
        posibile_guide_light.push_back(contour_idx);

        // record max
        if (contour_area[contour_idx] > max_contour_area) {
            max_contour_area = contour_area[contour_idx];
            // max_contour_idx = max_contour_idx;
            max_contour_idx = contour_idx;
        }
    }

    if (posibile_guide_light.size() == 0) {
        return cv::Point3f(640, 512, 0);
    } else {
        // find the best guide light
        cv::Point3f guide_light;
        cv::Point2f result = bound_rect[max_contour_idx].center;
        guide_light.x = result.x; guide_light.y = result.y; guide_light.z = max_contour_area;
        cv::circle(img, result, bound_rect[max_contour_idx].size.width / 2.0, cv::Scalar(0, 0, 255), 2);

        cv::Point gl_center((int)result.x , (int)result.y);
        std::string hsv_info;
        hsv_info = "HSV: " + to_string(hsv.at<Vec3b>(gl_center)[0]) + ", " + to_string(hsv.at<Vec3b>(gl_center)[1]) + ", " + to_string(hsv.at<Vec3b>(gl_center)[2]);
        cv::putText(img, hsv_info, Point((int)result.x + 50, (int)result.y + 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 4, 8);
        return guide_light;
    }
}