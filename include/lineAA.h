#pragma once
#include <vector>
#include "util.h"
#include "lineAA_sampling.h"

struct Pixel {
    int x,y;
    double alpha;
};

std::vector<Pixel> generateSmoothLine(double x1, double y1, double x2, double y2
, double thickness=1, double wide=1.0, int vsub=3, double gc=1, double imin=0, int ssMode=SS_GRID, double range=500);