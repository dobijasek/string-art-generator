#include "testQueue.h"

#include "util.h"
#include "pair.h"
#include "core.h"

extern volatile int randsTotal;
extern int vselector;
extern int vlayers;
extern int v0;
extern int rands;
extern core_t srcc;

void push(TSQueue<aaa_t> &queue, int start, int end, int usedIdx, int lid, int lie, scoreResult_t &sr_erase){
    lid = constrain(lid,0,srcc.layers()-1);
    lie = constrain(lie,0,srcc.layers()-1);
    queue.push({start,end,usedIdx,lid,lie,sr_erase});
}

void tqRandom(TSQueue<aaa_t> &queue, std::vector<scoreResult_t> &sr_worst_lst, int lidRange){
    randsTotal = rands;
    pairGenerator(srcc,0,[&](int start, int end,int usedIdx){
        auto &sr_erase = sr_worst_lst.at(random(0,sr_worst_lst.size()-1));
        int lie = sr_erase.layerIdxErase;
        int lid = lie;
        if(lidRange<0){
            lid = random(0,srcc.layers()-1);
        } else {
            lid = random(lie-lidRange,lie+lidRange);
        }
        push(queue,start,end,usedIdx,lid,lie,sr_erase);
    });
}

// void tqNear(TSQueue<aaa_t> &queue, std::vector<scoreResult_t> &sr_worst_lst, int lidRange){
void tqNear(TSQueue<aaa_t> &queue, std::vector<scoreResult_t> &sr_worst_lst, int lidRange, int nearRange){
    auto &sr_erase = sr_worst_lst.at(random(0,sr_worst_lst.size()-1));
    nearRange = nearRange<1 ? 1 : nearRange;
    pairGeneratorNear(srcc,srcc.getHist(sr_erase.layerIdxErase).start,srcc.getHist(sr_erase.layerIdxErase).end
    ,nearRange,[&](int start, int end,int usedIdx){
        // queue.push({start,end,usedIdx,sr_worst.layerIdxErase,sr_worst.layerIdxErase,sr_worst});
        if(v0>0){
            // for(int i=0;i<v0;i++){
            for(int i=-v0;i<v0;i++){
                int lid = sr_erase.layerIdxErase + i;
                int lie = sr_erase.layerIdxErase;
                // int lid = random(0,srcc.layers()-1);
                push(queue,start,end,usedIdx,lid,lie,sr_erase);
                int lid2 = random(0,srcc.layers()-1);
                push(queue,start,end,usedIdx,lid2,lie,sr_erase);
            }
        } else {
            // int lid = sr_erase.layerIdxErase;
            int lie = sr_erase.layerIdxErase;
            int lid = lidRange==0 ? lie : random(lie-lidRange,lie+lidRange);
            // int lie = random(0,srcc.layers()-1);
            push(queue,start,end,usedIdx,lid,lie,sr_erase);
        }
    });
}

void tq(TSQueue<aaa_t> &queue, std::vector<scoreResult_t> &sr_worst_lst){
    randsTotal = rands;
    int select = random(1,100);
    if(select>vselector){
        pairGenerator(srcc,0,[&](int start, int end,int usedIdx){
            // queue.push({start,end,usedIdx,sr_worst.layerIdxErase,sr_worst.layerIdxErase,sr_worst});
            auto &sr_worst = sr_worst_lst.at(random(0,sr_worst_lst.size()-1));
            if(v0>0){
                for(int i=-v0;i<v0;i++){
                    int lid = sr_worst.layerIdxErase + i;
                    int lie = sr_worst.layerIdxErase;
                    // int lie = random(0,srcc.layers()-1);
                    lid = constrain(lid,0,srcc.layers()-1);
                    lie = constrain(lie,0,srcc.layers()-1);
                    queue.push({start,end,usedIdx,lid,lie,sr_worst});
                }
            } else {
                // int lid = sr_worst.layerIdxErase;
                int lie = sr_worst.layerIdxErase;
                int lid = random(0,srcc.layers()-1);
                // int lid = random(lie-30,lie+30);
                lid = constrain(lid,0,srcc.layers()-1);
                lie = constrain(lie,0,srcc.layers()-1);
                queue.push({start,end,usedIdx,lid,lie,sr_worst});
            }
            // for(int i=0;i<random(0,vlayers);i++){
            //     queue.push({start,end,usedIdx,random(0,srcc.layers()-1),sr_worst.layerIdxErase,sr_worst});
            // }
            // queue.push({start,end,usedIdx,sr_worst.layerIdxErase,sr_worst.layerIdxErase,sr_worst});
        });
    } else {
    // if(select<=vselector){
        int lmax = vlayers<=1 ? 2 : random(1,vlayers);

        auto &sr_worst = sr_worst_lst.at(random(0,sr_worst_lst.size()-1));
        pairGeneratorNear(srcc,srcc.getHist(sr_worst.layerIdxErase).start,srcc.getHist(sr_worst.layerIdxErase).end,2,[&](int start, int end,int usedIdx){
            // queue.push({start,end,usedIdx,sr_worst.layerIdxErase,sr_worst.layerIdxErase,sr_worst});
            if(v0>0){
                for(int i=-v0*3;i<v0*3;i++){
                    int lid = sr_worst.layerIdxErase + i;
                    int lie = sr_worst.layerIdxErase;
                    // int lie = random(0,srcc.layers()-1);
                    lid = constrain(lid,0,srcc.layers()-1);
                    lie = constrain(lie,0,srcc.layers()-1);
                    queue.push({start,end,usedIdx,lid,lie,sr_worst});
                }
            } else {
                int lid = sr_worst.layerIdxErase;
                int lie = sr_worst.layerIdxErase;
                // int lie = random(0,srcc.layers()-1);
                lid = constrain(lid,0,srcc.layers()-1);
                lie = constrain(lie,0,srcc.layers()-1);
                queue.push({start,end,usedIdx,lid,lie,sr_worst});
            }
            // queue.push({start,end,usedIdx,random(0,srcc.layers()-1),random(0,srcc.layers()-1),sr_worst});
        });
    }
}