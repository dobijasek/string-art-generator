#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include "nail.h"

inline float mapfc(float x,float il,float ih,float ol,float oh){
  float out = (x-il)*(oh-ol)/(ih-il)+ol;
  out = out>oh?oh:out;
  out = out<ol?ol:out;
  return out;
}

inline float sign(float val){
    return val<0 ? -1 : 1;
}

inline double mapf(double x,double il,double ih,double ol,double oh){
    return (x-il)*(oh-ol)/(ih-il)+ol;
}

inline float constrain(float x, float l, float h){
  return (x>h) ? h : ((x<l) ? l : x);
}

inline int constrain(int x, int l, int h){
  return (x>h) ? h : ((x<l) ? l : x);
}

inline int constrain(uint16_t x, uint16_t l, uint16_t h){
  return (x>h) ? h : ((x<l) ? l : x);
}

inline float mean(uchar *data, int length){
    float total = 0;
    for(int i = 0; i < length; i++){
        total = total + data[i];
    }
    return (total/length);
}

inline float mean(int *data, int length){
    float total = 0;
    for(int i = 0; i < length; i++){
        total = total + data[i];
    }
    return (total/length);
}


// sum up the square of the residual 
inline float total_sum_of_square(uchar *y, int length){
     
    float total = 0;
    float y_mean = mean(y,length);

    for(int i = 0 ; i < length; i++){
        float residual = y[i] - y_mean;
        total = total + (residual*residual);
    }
    return total;
}

// sum up the residual of the squared errors
inline float residual_sum_of_square(uchar *y_pred, uchar *y_true, int length){
    float total = 0;

    for(int i = 0 ; i < length; i++){
        float residual = y_true[i] - y_pred[i];
        total = total + (residual*residual);
    }
    return total;
}

// Coefficient of determination for goodness of fit of the regression
inline float r2(uchar *y_pred, uchar *y_true, int length){
    float sum_squared_residual = residual_sum_of_square(y_pred,y_true,length);
    float sum_squared_total = total_sum_of_square(y_true,length);
    // cout << sum_squared_residual << "     " << sum_squared_total << "   " << mean(y_true,length);
    // printf("     %u %u %u %u %u   %u %u %u %u %u\n",y_pred[0],y_pred[1],y_pred[2],y_pred[3],y_pred[4],y_true[0],y_true[1],y_true[2],y_true[3],y_true[4]);
    return 1.0 - (sum_squared_residual/sum_squared_total);
    // return  sum_squared_residual/sum_squared_total;
}
#include <random>
inline int random(int min, int max){
    if(min==max) return min;
    static thread_local std::mt19937 generator;
    std::uniform_int_distribution<int> distribution(min,max);
    return distribution(generator);
}

struct uniquePair_t
{
    int a;
    int b;
};
inline uniquePair_t randomUniquePairDiff(int lo, int hi, int diff){
    int a = random(lo,hi);
    int dir = random(0,1) ? 1 : -1;
    int d = random(1,diff);
    int r = a+d*dir;
    int b = constrain(r,lo,hi);
    return {a,b};
}

inline double sign(double v){
    return v>=0 ? 1 : -1;
}

inline double randomf(double min, double max){
    static std::random_device rd;  // Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<double> dis(min,max);
    return dis(gen);
}

inline float genTriangle(float bin, float period, float l, float h, float noise=0){
    return mapf(asin(sin(2*M_PI*1*bin/period))/M_PI*2,-1,1,l,h)+randomf(-noise,noise);
}

// https://www.youtube.com/watch?v=iATjsfAX8yc