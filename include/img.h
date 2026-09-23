#pragma once
#include <opencv2/opencv.hpp>

class img_t
{
    cv::Mat img;
    public:
    void init(int w, int h){
        img.create(h,w,CV_32F);
    }

    int w(){
        return img.cols;
    }

    int h(){
        return img.rows;
    }

    int size(){
        return w()*h();
    }

    float at(int x, int y){
        return img.at<float>(y,x);
    }

    void at(int x, int y, float val){
        img.at<float>(y,x) = val;
    }

    void set(float value){
        img = value;
    }
};