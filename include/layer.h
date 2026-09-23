#pragma once
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include "layerData.h"
#include <immintrin.h>

struct objective_t
{
    double o1;
    double o2;
    int best;
};

struct layer_t
{
    lil_t *lil;
    double cw;
    double intensity;
    layer_t(lil_t *lil, double cw, double intensity)
    : lil(lil), cw(cw), intensity(intensity){
    };
};

struct outData_t
{
    double cdInsert;
    double cdErase;
    double cs;
    double cw;
    int layerIdxDraw;
    int layerIdxErase;
    double contribI;
    double contribE;
    double weight;
    int offset;
    outData_t(double cdI, double cdE, double cs, double cw, int layerIdxDraw, int layerIdxErase
        , double contrib, double contribE
        , double weight)
    : cdInsert(cdI), cdErase(cdE), cs(cs), cw(cw)
    , layerIdxDraw(layerIdxDraw), layerIdxErase(layerIdxErase)
    , contribI(contrib), contribE(contribE), weight(weight){};
    outData_t(){
        outData_t(0,0,0,0,0,0,0,0,0);
    }
};

class computePixel_t
{  
    public:
    std::vector<layer_t> layer;
    double cs;
    double cb;
    double *weight;

    computePixel_t(double cs, double cb,double *pweight) : cs(cs), cb(cb), weight(pweight)
    {
    }

    int dense() const {
        return layer.size();
    }

    int layerIdxLast() const {
        return layer.size()>0 ? (layer.back().lil->layerIdx) : -1;
    }

    inline void insert(lil_t *l, double cw, double intensity) {
        auto f = [](const layer_t& a,const layer_t& b) { return a.lil->layerIdx < b.lil->layerIdx; };
        const auto layerNew = layer_t(l,cw,intensity);
        auto it = std::lower_bound(layer.begin(), layer.end(), layerNew, f);
        layer.insert(it, layerNew);
    }

    inline void erase(int layerIdx) {
        auto f = [&layerIdx](const layer_t& elem) { return elem.lil->layerIdx == layerIdx; };
        auto it = std::find_if(layer.begin(), layer.end(), f);
        if(it != layer.end()) {
            layer.erase(it);
        } else {
            // printf("błąd erase(%i)\n",layerIdx);
        }
    }

    double color() const;
    double color2() const;
    std::vector<outData_t> dataTestv3(const std::vector<double> &cw_, double intensity, int layerIdxDraw, int layerIdxErase) const;
    std::vector<outData_t> dataTestv2(const std::vector<double> &cw_, double intensity, int layerIdxDraw, int layerIdxErase) const;
    std::vector<outData_t> dataTestv(const std::vector<double> &cw_, double intensity, int layerIdxDraw, int layerIdxErase) const;
};