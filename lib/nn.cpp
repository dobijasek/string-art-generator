#include "nn.h"
#include <math.h>

float softplus(float x){
    return log(1.0+exp(x));
}

float softplus_derivative_fromX(float x){
    return exp(x)/(1.0+exp(x));
}

float softplus_derivative_fromOut(float exp_x){
    return exp_x/(1.0+exp_x);
}

float sigmoid(float x){
  return 1.0/(1.0+exp(-x));
}

float sigmoid_derivative(float x){
  return x * (1.0 - x);
}

// float tanh_derivative_fromX(float x){
//   float cosx = std::cos(x);
//   return 1.0 / (cosx*cosx);
// }

float tanh_derivative_fromX(float x) {
    float tanh_x = std::tanh(x);
    return tanh_derivative_fromOut(tanh_x);
}

float tanh_derivative_fromOut(float tanh_x) {
    return 1.0 - tanh_x * tanh_x;
}

static const float absQ_C = 1000;
double sech(double x) {
    return 1.0 / std::cosh(x);
}

double sech2(double x) {
    double coshx = std::cosh(x);
    return 1.0 / (coshx * coshx);
}

float abs_derivative_fromX(float x){
  if (x > 0) return 1.0;
  else if (x < 0) return -1.0;
  else return 0.0;
}

float abs1(float x) {
    return abs(x);
}

float abs1_derivative(float x) {
    return abs_derivative_fromX(x);
}

float abs2(float x) {
    return x * std::tanh(x);
}

float abs2_derivative(float x) {
    float tanh_x = std::tanh(x);
    return tanh_x + x * (1.0 - tanh_x * tanh_x);
}

float absQ(float x) {
    return abs1(x);
}

float absQ_derivative(float x) {
    return abs1_derivative(x);
}

cv::Mat loss(cv::Mat &out, cv::Mat &target){
    cv::Mat outf = out.clone();
    cv::Mat targetf = target.clone();
    outf.convertTo(outf,CV_32F,1.0/255.0,0);
    targetf.convertTo(targetf,CV_32F,1.0/255.0,0);
    cv::Mat gradient = -(targetf-outf);
    gradient = gradient;
    return gradient;
}