#pragma once
#include "wire.h"
#include "nail.h"
#include "core.h"
#include "score_t.h"
#include "line.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include "util.h"
#include "paerson.h"

float sigmoid(float x);

float sigmoid_derivative(float f);

scoreResult_t getScorePixel(int x, int y, const core_t &c, int wireIdx, int method);
// scoreResult_t getPixelScore(int imgOffset,wire_t &wire,int layerIdx,uint8_t *src,uint8_t *dst,float *bias,uint16_t *layer);
scoreResult_t getScoreLine(int start, int end, core_t &c, int wireIdx, const scoreMode_t &method);
double getImgErrorProcentage(cv::Mat &src, cv::Mat &dst, cv::Mat &map, double q1);
double getImgErrorProcentage2(cv::Mat &src, cv::Mat &dst, cv::Mat &map, double q1);
double getImgPaerson(cv::Mat &src, cv::Mat &dst, cv::Mat &map, double q1);

inline double square(double x){
  return x*x;
}
extern int vtanh;
inline double pscore(double s, double d,double q1){
  double diff = abs(s-d);
  // double diffScaled = mapf(diff,0,vtanh,0,3);
  // double diffTanh = tanh(diff);
  // diff = diffTanh;
  // return diff;
  return pow(diff,q1);
}

inline double pscoreTanh(double s, double d,double q1){
  double diff = abs(s-d);
  double diffScaled = mapf(diff,0,vtanh,0,2);
  double diffTanh = tanh(diff);
  // diff = diffTanh;
  // return diff;
  return pow(diff,q1);
}

class scoreParam_t
{
  public:
  double powexp = 1;
  int method = 1;
  int normalize = 0;
  void randomize(double pl, double ph){
    powexp = randomf(pl,ph);
    // method = random(0,1);
    normalize = random(0,1);
  }
};

int isBottom(int scoreMethod);

scoreResult_t getLineScore(int color,line_c &line,const cv::Mat &src,const cv::Mat &dst,const cv::Mat &layer, int layerIdx);
std::vector<scoreResult_t> getLineScoreqq(const std::vector<double> &color,int start,int end, int layerIdxDraw, int layerIdxErase);
std::vector<scoreResult_t> getLineScore2(const std::vector<double> &color,line_c &line, int layerIdxDraw, int layerIdxErase);