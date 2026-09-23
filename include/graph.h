#pragma once
#include <opencv2/opencv.hpp>

class graph
{
    public:
    cv::Mat m;
    int w;
    int h;
    graph(int w, int h);
    // void addPlot();
};

void graphPlot(std::vector<double>&,std::vector<double>&);