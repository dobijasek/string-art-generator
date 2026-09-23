#pragma once
#include <opencv2/opencv.hpp>

class wire_t
{
    public:
    int nail  = 0;   // numer haka na którym aktualnie znajduje się sznurek
    int side  = 0;   // po której stronie kończy się sznurek
    double color;    // kolor sznurka
    const int index; // index sznurka
    
    cv::Vec3b colorBGR;

    wire_t(double color, int index);
};