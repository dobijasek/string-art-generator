#pragma once
#include <opencv2/opencv.hpp>
#include "layer.h"
#include "line.h"
#include "wire.h"
#include "layerData.h"

class composite_t
{
    public:
    int w;
    int h;
    std::vector<computePixel_t> layerq;
    std::list<lil_t> lil;
    cv::Mat raw64;
    cv::Mat src64;
    cv::Mat rng;
    void init(cv::Mat src, cv::Mat WEIGHT);
    void randn(double mean, double stddev);
    void randnw(double mean, double stddev);
    void preupdate();
    void scale(double lo, double hi);
    void smooth(double smoothness);
    void update();
    lil_t* insert(int start, int end, int usedIdx, line_c &line, wire_t &wire, int layerIdx);
    bool validateLst(const char *s,int layerIdx);
    void arrangeLst();
    void erase(int layerIdx);
    computePixel_t& at(int px);
    computePixel_t& at(int x, int y);
    void clear();

};