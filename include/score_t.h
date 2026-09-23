#pragma once
#include "wire.h"
#include "paerson.h"

class scoreResult_t
{
    public:
    int empty = 1;
    double score  = 0;
    double score2 = 0;
    double score3 = 0;
    double score4 = 0;
    double _emptys = 0;
    Paerson__ pearson;
    double processed = 0;
    double contribI = 0;
    double contribE = 0;
    double len = 0; // TODO: const...
    double scoreE = 0;
    double scoreI = 0;
    double cs = 0;
    double cdE = 0;
    double cdI = 0;
    int start = 0;
    int end = 0;
    int layerIdxDraw = 0;
    int layerIdxErase = 0;
    wire_t *wire = nullptr;

    scoreResult_t& operator+=(const scoreResult_t &v){
        score     += v.score;
        processed += v.processed;
        len += v.len;
        cs += v.cs;
        cdE += v.cdE;
        cdI += v.cdI;
        scoreE += v.scoreE;
        scoreI += v.scoreI;
        return *this;
    }
};

inline bool cmp(const scoreResult_t &a,const scoreResult_t &b){
    bool a_less_b = a.score<b.score && b.score2<=b.score2 && a.score3<=b.score3;
    return a_less_b;
}

struct pixelData_t
{
    float cw;
    float cs;
    float cd;
    float dense;
    float weightSrc;
    float weightDst;
    float weightDense;
    float bias;
    float biasSrc;
    float biasDst;
};

struct scoreMode_t
{
    int pixel;
    int norm;
    int fast;
};

enum PIXELMODE
{
    PIXEL_M1,
    PIXEL_M2,
    PIXEL_COUNT,
    PIXEL_WOW,
    PIXEL_WOW2,
    PIXEL_WOW3,
    PIXEL_BOTTOM,
    PIXEL_SSUM,
    PIXEL_DSUM,
    PIXEL_ABSCSCW,
    PIXEL_ABSCDCW,
    PIXEL_ABSCSCD
};