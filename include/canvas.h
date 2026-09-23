#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include "nail.h"
#include "wire.h"
#include "composite.h"

class canvas_t
{
    public:
    int bval; // TODO: do wywalenia?
    int w;
    int h;
    cv::Mat img;
    cv::Mat weight;
    cv::Mat dense;
    cv::Mat cover;
    cv::Mat layerNum;
    composite_t comp;
    // std::vector<layerq_t> layerq;
    cv::Mat getDenseq();
    int layerIdx;
    std::vector<nail_t> nail;
    std::vector<wire_t> wire;
    void init(int nails, int sides, const std::vector<double> &color, int w, int h);
    void drawLine(int start, int end, int wireIdx);
    void drawLine(cv::Mat &m, int start, int end, int wireIdx);
    void eraseLine(int start, int end, int LI);
    void reset(int val);
};