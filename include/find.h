#pragma once
#include "best.h"
#include "core.h"

void testPair(best_t &best, int start, int end, int usedIdx, core_t &c, int wireIdx);
best_t find(core_t &c);