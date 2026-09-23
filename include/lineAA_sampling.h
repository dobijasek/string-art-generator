#pragma once
#include <random>
#include <functional>
#include "util.h"

#define SS_GRID         0
#define SS_GRIDE        1
#define SS_RANDOM       2
#define SS_RANDOM_COUNT 3
#define SS_MODES        4

inline double random_(double min, double max){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(min,max);
    return dis(gen);
}

inline void supersampling(int divs, const std::function<void(double x,double y)>& f, int ssMode=SS_GRID, double range=500) {
    if(ssMode<0) f(0,0);
    if(ssMode==SS_RANDOM_COUNT){
        int q = round(random_(1,divs*divs));
        for(int i=0;i<q;i++){
            double subpixelX = random_(-0.5,0.5);
            double subpixelY = random_(-0.5,0.5);
            f(subpixelX,subpixelY);
        }
    }
    if(ssMode==SS_RANDOM){
        int q = divs*divs;
        for(int i=0;i<q;i++){
            double subpixelX = random_(-0.5,0.5);
            double subpixelY = random_(-0.5,0.5);
            f(subpixelX,subpixelY);
        }
    }
    if(ssMode==SS_GRID){
        for(int subY=0; subY<divs; subY++) {
            for(int subX=0; subX<divs; subX++) {
                double subpixelX = (double)(1+2*subX-divs)/(2*divs);
                double subpixelY = (double)(1+2*subY-divs)/(2*divs);
                f(subpixelX,subpixelY);
            }
        }
    }
    double q1 = 1.0/(divs*2);
    double q2 = 1.0/(divs);
    double q3 = (1-divs)/(divs*2); // -0.5+q1;
    if(ssMode==SS_GRIDE){
        for(int subY=0; subY<divs; subY++) {
            for(int subX=0; subX<divs; subX++) {
                double subpixelX = mapf(subX,0,divs-1,-range,range);
                double subpixelY = mapf(subY,0,divs-1,-range,range);
                f(subpixelX,subpixelY);
            }
        }
    }
}