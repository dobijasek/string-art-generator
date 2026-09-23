#include "find.h"
#include "score.h"
#include "opencvex.h"
#include "pool.h"
#include "pair.h"

void testPair(best_t &best, int start, int end, int usedIdx, core_t &c, int wireIdx){
    // int usedIdxx = getNailsIndex(start,end,c.dst.nail.size());
    // float cwx =  c.dst.wire.at(wireIdx).color;
    // float scorex = c.line.at(usedIdxx).getScore(cwx);
    // float processedx = c.line.at(usedIdxx).len;
    // best.testScore(scorex,processedx,start,end,usedIdx,c.dst.wire.at(wireIdx));
    auto sr = getScoreLine(start,end,c,wireIdx,c.scoreMode);
    best.testScore(sr.score,sr.len,sr.processed,start,end,usedIdx,c.dst.wire.at(wireIdx));
}

extern int volatile randsTotal;
extern int rands;
// best_t find3(core_t &c){
    // randsTotal = rands;
    // best_t best;
    // std::vector<uint8_t> color(c.dst.wire.size());
    // for(int i=0;i<color.size();i++){
    //     color[i] = c.dst.wire[i].color;
    // }
    // for(int i=0;i<TASKER.threads(1);i++)
    // TASKER.push({[&c,&color,&best](){
    //     pairGenerator(c,0,[&best,&c,&color](int start, int end,int usedIdx){
    //         std::vector<scoreResult_t> sr = getLineScore(color,c.line.at(usedIdx),c.src,c.dst.img,c.dst.layer,c.lineIdx);
    //         for(int wireIdx=0;wireIdx<sr.size();wireIdx++){
    //             auto s = sr[wireIdx];
    //             best.testScore(s.score,s.len,s.processed,start,end,usedIdx,c.dst.wire.at(wireIdx));
    //         }
    //     });
    // }});
    // TASKER.wait();
    // return best;
// }

// best_t find2(core_t &c){
//     randsTotal = rands;
//     best_t best;
//     std::vector<uint8_t> color(c.dst.wire.size());
//     for(int i=0;i<color.size();i++){
//         color[i] = c.dst.wire[i].color;
//     }
//     for(int i=0;i<TASKER.threads(1);i++)
//     TASKER.push({[&c,&color,&best](){
//         pairGenerator(c,0,[&best,&c,&color](int start, int end,int usedIdx){
//             std::vector<scoreResult_t> sr = getLineScore(color,c.line.at(usedIdx),c.src,c.dst.img,c.dst.layer,c.lineIdx);
//             for(int wireIdx=0;wireIdx<sr.size();wireIdx++){
//                 auto s = sr[wireIdx];
//                 best.testScore(s.score,s.len,s.processed,start,end,usedIdx,c.dst.wire.at(wireIdx));
//             }
//         });
//     }});
//     TASKER.wait();
//     return best;
// }

best_t find1(core_t &c){
    randsTotal = rands;
    std::vector<best_t> wiresBest(c.dst.wire.size());
    for(int wireIdx=0;wireIdx<c.dst.wire.size();wireIdx++){
        TASKER.push({[&wiresBest,&c,wireIdx](){
            pairGenerator(c,wireIdx,[&wiresBest,&c,wireIdx](int start, int end,int usedIdx){
                testPair(wiresBest.at(wireIdx),start,end,usedIdx,c,wireIdx);
            });
        }});
    }
    TASKER.wait();
    best_t best = best_t{wiresBest};
    best.scoreMode = c.scoreMode;
    return best;
}

best_t find(core_t &c){
    return find1(c);
}