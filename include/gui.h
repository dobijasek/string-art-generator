#pragma once
#include <opencv2/opencv.hpp>
#include <string>
void guiCreate();
void guiDestroy();

struct guiPush
{
    std::string winname;
    cv::Mat mat;
    int mode;
    guiPush(const std::string name,const cv::Mat m,int mode)
        : winname(name), mat(m.clone()), mode(mode){}
};

void imshow_(const std::string &winname,const cv::Mat &mat,int mode=0);