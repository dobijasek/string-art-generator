#pragma once
#include "line.h"
#include "wire.h"
#include "score_t.h"

struct lil_t
{
    int start;
    int end;
    int usedIdx;
    line_c *p_line;
    wire_t *p_wire;
    int layerIdx;
    int drop;
    scoreResult_t sr;
    int outlier;
};