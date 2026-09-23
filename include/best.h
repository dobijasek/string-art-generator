#pragma once
#include "wire.h"
#include <vector>
#include "score_t.h"

class best_t
{
    public:
    double score;
    double score2;
    double score3;
    scoreResult_t sr;
    double len;
    double processed;
    int usedIdx;
    int start;
    int end;
    int layerIdxDraw;
    int layerIdxErase;
    wire_t *wire;
    scoreMode_t scoreMode;
    std::vector<float> cs;
    std::vector<float> cd;
    std::vector<float> dense;
    std::vector<float> weightSrc;
    std::vector<float> weightDst;
    std::vector<float> weightDense;
    std::vector<float> bias;
    std::vector<float> biasSrc;
    std::vector<float> biasDst;

    best_t();
    best_t(std::vector<best_t> &bestList);
    void testScore(std::vector<best_t> &bestList);
    void testScore(best_t &best);
    void testScore(float Score, float Len, float Processed, int Start, int End, int UsedIdx, wire_t &Wire);
    void testScoreR(double Score, double Len, double Processed, int Start, int End, int UsedIdx, wire_t &Wire, int layerIdxDraw, int layerIdxErase);
    void testScoreR2(double Score, double Len, double Processed, int Start, int End, int UsedIdx, wire_t &Wire, int layerIdxDraw, int layerIdxErase);
    void testScoreS3(int inv, scoreResult_t &Sr, double Score, double Score2, double Score3, int Start, int End, int UsedIdx, wire_t &Wire, int layerIdxDraw, int layerIdxErase);
    void testScoreNot(float Score, float Len, float Processed, int Start, int End, int UsedIdx, wire_t &Wire);
    void testScore2(float Score, float Len, float Processed, int Start, int End, int UsedIdx, wire_t &Wire);
    void reset();
    bool isEmpty();
    int found();
};