#pragma once
#include <opencv2/opencv.hpp>
#include "nail.h"
#include "wire.h"
#include <string>
#include "best.h"
#include "score_t.h"
#include "canvas.h"
#include "line.h"

struct scoreq_t
{
    scoreResult_t *sr;
    wire_t *wire;
    int imgOffset;
};

struct cws_t
{
    uint8_t cw[4];
};

class core_t
{    
    public:
    int w;
    int h;
    // cv::Mat img;
    cv::Mat raw;
    cv::Mat src;
    canvas_t dst;
    cv::Mat dst_;
    cv::Mat dst2;
    cv::Mat dstDone;
    cv::Mat used;
    cv::Mat mask;
    cv::Mat maskDense;
    cv::Mat layerDense;
    uint8_t c2nearestIdx[256];
    int sides;
    std::vector<line_c> line;
    std::vector<double> colors;
    int getLineId(int start, int end);
    line_c& getLine(int start, int end);
    line_c& getLine(int lineId);
    scoreMode_t scoreMode;
    int lineIdx;
    std::vector<best_t> hist;
    
    std::vector<double> getColors();
    std::vector<std::vector<scoreq_t>> scoreq;
    std::vector<cws_t> cws;

    std::vector<double> getColorsFromSrc(int wires);
    void init(std::string filename, int Nails, int Sides, int Wires);
    void reset(int Bval);
    void drawLineBP(best_t &best);
    void drawLine(best_t &best);
    lil_t& getHist(int layerIdx);
    void drawLine3(int start, int end, wire_t &wr, int LII);
    lil_t eraseLine3(int layerIdxErase);
    void createMask();
    void line_init();
    scoreResult_t getLineScore_(int color, int lineIdx, int layerIdx=-1);
    scoreResult_t getLineScoreFromLayer(int layerIdx);
    int layers(){ return dst.comp.lil.size(); }

    private:
    void drawLine(int start, int end, int wireIdx);
};