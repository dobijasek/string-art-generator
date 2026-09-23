#include "best.h"
#include <float.h>
#include "Instrumentor.h"

best_t::best_t(){
    reset();
}

best_t::best_t(std::vector<best_t> &bestList){
    reset();
    testScore(bestList);
}

void best_t::reset(){
    score = DBL_MAX;
    score2 = DBL_MAX;
    score3 = DBL_MAX;
    len = DBL_MAX;
    processed = DBL_MAX;
    usedIdx = -1;
    start = 0;
    end = 0;
    wire = NULL;
    layerIdxDraw = -1;
    layerIdxErase = -1;
    scoreMode = {-1,-1,-1};
    cs.clear();
    cd.clear();
    weightSrc.clear();
    weightDst.clear();
    bias.clear();
    biasSrc.clear();
    biasDst.clear();
}

bool best_t::isEmpty(){
    return score==DBL_MAX;
}

void best_t::testScore(best_t &best){
    testScore(best.score
             ,best.len
             ,best.processed
             ,best.start
             ,best.end
             ,best.usedIdx
             ,*best.wire);
}

void best_t::testScore(std::vector<best_t> &bestList){
    for(auto best : bestList){
        testScore(best);
    }
}
static std::mutex m_mutex;
void best_t::testScore(float Score, float Len, float Processed, int Start, int End, int UsedIdx, wire_t &Wire){
    // PROFILE_FUNCTION();
    // if(Len<=0) return;
    std::lock_guard<std::mutex> lock(m_mutex);
    // assert(Len!=0);
    if(Score<score){
        score     = Score;
        len       = Len;
        processed = Processed;
        start     = Start;
        end       = End;
        usedIdx   = UsedIdx;
        // assert(&Wire!=nullptr);
        wire      = &Wire;
    }
}

void best_t::testScoreR(double Score, double Len, double Processed, int Start, int End, int UsedIdx, wire_t &Wire, int LayerIdxDraw, int LayerIdxErase){
    std::lock_guard<std::mutex> lock(m_mutex);
    if(Score<score){
        score         = Score;
        len           = Len;
        processed     = Processed;
        start         = Start;
        end           = End;
        usedIdx       = UsedIdx;
        wire          = &Wire;
        layerIdxDraw  = LayerIdxDraw;
        layerIdxErase = LayerIdxErase;
    }
}

void best_t::testScoreR2(double Score, double Len, double Processed, int Start, int End, int UsedIdx, wire_t &Wire, int LayerIdxDraw, int LayerIdxErase){
    std::lock_guard<std::mutex> lock(m_mutex);
    if(Score<score && Len<len && Processed<processed){
        score         = Score;
        len           = Len;
        processed     = Processed;
        start         = Start;
        end           = End;
        usedIdx       = UsedIdx;
        wire          = &Wire;
        layerIdxDraw  = LayerIdxDraw;
        layerIdxErase = LayerIdxErase;
    }
}

void best_t::testScoreS3(int inv, scoreResult_t &Sr, double Score, double Score2, double Score3, int Start, int End, int UsedIdx, wire_t &Wire, int LayerIdxDraw, int LayerIdxErase){
    std::lock_guard<std::mutex> lock(m_mutex);
    bool pass = Score<score && Score2<=score2 && Score3<=score3;
    bool passInv = Score>score && Score2>=score2 && Score3>=score3;
    if(inv ? passInv : pass){
        score         = Score;
        score2        = Score2;
        score3        = Score3;
        sr            = Sr;
        // len           = Len;
        // processed     = Processed;
        start         = Start;
        end           = End;
        usedIdx       = UsedIdx;
        wire          = &Wire;
        layerIdxDraw  = LayerIdxDraw;
        layerIdxErase = LayerIdxErase;
    }
}

void best_t::testScoreNot(float Score, float Len, float Processed, int Start, int End, int UsedIdx, wire_t &Wire){
    // PROFILE_FUNCTION();
    if(Len<=0) return;
    std::lock_guard<std::mutex> lock(m_mutex);
    // assert(Len!=0);
    if(Score>score){
        score     = Score;
        len       = Len;
        processed = Processed;
        start     = Start;
        end       = End;
        usedIdx   = UsedIdx;
        // assert(&Wire!=nullptr);
        wire      = &Wire;
    }
}

void best_t::testScore2(float Score, float Len, float Processed, int Start, int End, int UsedIdx, wire_t &Wire){
    // PROFILE_FUNCTION();
    std::lock_guard<std::mutex> lock(m_mutex);
    assert(Len!=0);
    if(Score<score){
        score     = Score;
        len       = Len;
        processed = Processed;
        start     = Start;
        end       = End;
        usedIdx   = UsedIdx;
        // assert(&Wire!=nullptr);
        wire      = &Wire;
    }
}

int best_t::found(){
    // return usedIdx==-1?0:1;
    return score==DBL_MAX?0:1;
}