#pragma once
#include "wire.h"
#include "queue.h"
#include "score_t.h"

struct aaa_t
{
    int start;
    int end;
    int usedIdx;
    int layerIdxDraw;
    int layerIdxErase;
    scoreResult_t sr_erase;
};

void tq(TSQueue<aaa_t> &queue, std::vector<scoreResult_t> &sr_worst_lst);
void tqRandom(TSQueue<aaa_t> &queue, std::vector<scoreResult_t> &sr_worst_lst, int lidRange);
void tqNear(TSQueue<aaa_t> &queue, std::vector<scoreResult_t> &sr_worst_lst, int lidRange, int nearRange);