#pragma once
#include <opencv2/opencv.hpp>

struct hists_t
{
    cv::Mat mix;
    cv::Mat raw;
    cv::Mat src;
    cv::Mat dst;
};

cv::Mat getHistogram(const hists_t &mats);