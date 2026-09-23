#pragma once
#include <cmath>
#include <algorithm>
#include <float.h>

class varMetric
{
    double N = 0;
    double S1 = 0;
    double S2 = 0;
    double min = DBL_MAX;
    double max = DBL_MIN;

    void apply(double v){
        N += 1;
        S1 += pow(v,1);
        S2 += pow(v,2);
        min = std::min(min,v);
        max = std::max(max,v);
    }

    double mean() const {
        return N > 0 ? S1/N : 0.0;
    }

    double var() const {
        if (N <= 0) return 0.0;
        double v = pow(S2/N,1) - pow(S1/N,2);
        return v > 0 ? v : 0.0;
    }

    double stddev() const {
        return sqrt(var());
    }

};