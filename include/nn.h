#pragma once
#include <opencv2/opencv.hpp>

float softplus(float x);
float softplus_derivative_fromX(float x);
float softplus_derivative_fromOut(float exp_x);
float abs_derivative_fromX(float x);
float sigmoid(float x);
float sigmoid_derivative(float x);
float tanh_derivative_fromX(float x);
float tanh_derivative_fromOut(float tanh_x);
float abs2(float x);
float absQ(float x);
float absQ_derivative(float x);
cv::Mat loss(cv::Mat &out, cv::Mat &target);