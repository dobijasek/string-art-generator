#pragma once
#include "wire.h"
#include "nail.h"
#include "core.h"
#include <functional>

void startEndNormalize(int &start, int &end, core_t &c);
void thruPair(int endPrior, int points_count, std::vector<nail_t> &nail, uint8_t *used, int w_val, int fast, int point_gap, const std::function<void(int,int,int)>& f);
void thruPairAll(int nailCnt, const std::function<void(int,int,int)>& f);
void pairGenerator(core_t &c, int wireIdx, const std::function<void(int,int,int)>& f);
void pairGeneratorNear(core_t &c, int startI, int endI, int bb, const std::function<void(int,int,int)>& f);