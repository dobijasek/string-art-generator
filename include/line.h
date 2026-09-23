#pragma once
#include <functional>
#include "nail.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include "score_t.h"

struct linePixel_t
{
    uint16_t x;
    uint16_t y;
    float intensity;
    uint32_t imgOffset;
    uint32_t usedIdx;
    // scoreResult_t sr;
};

#include "best.h"
#include "wire.h"
typedef const std::function<void(int,int,double)> tl_t;
class line_c
{
    private:
    std::vector<linePixel_t> pixel;
    public:
    float len;
    float score;
    int s;
    int e;
    std::vector<nail_t> *np;
    cv::Mat *srcp;
    cv::Mat *mp;
    void init(cv::Mat &src,cv::Mat &dst,cv::Mat &mask
        ,std::vector<nail_t> &nail, int start, int end);
    std::vector<linePixel_t> pixelGenerate();
    std::vector<linePixel_t> pixelLoad();
    void pixelSave(std::vector<linePixel_t> pp);
    void pixelClear();

    void thruLine_(int start, int end, tl_t& f);
};

void thruLine(int x1,int y1,int x2,int y2, tl_t& f);
void thruLine(int start, int end,const std::vector<nail_t> &nail,tl_t& f);
void thruLineAA(int start, int end,const std::vector<nail_t> &nail,tl_t& f);
int getLineLen(int start, int end, std::vector<nail_t> &nail);
int getLineLenMask(int start, int end, std::vector<nail_t> &nail, cv::Mat *mask);
int getLineLen2(int start, int end, std::vector<nail_t> &nail);
void getLine(cv::Mat *m, int *arr, int x1, int y1, int x2, int y2);
float getLineMean(cv::Mat *m, int start, int end, std::vector<nail_t> &nail);
void getLineStats(cv::Mat *m, cv::Mat *mask, int start, int end, std::vector<nail_t> &nail, float *min, float *max, float *mean, float *len);
float getLineMeanf(cv::Mat *m, int start, int end, std::vector<nail_t> &nail);