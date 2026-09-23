#include "pair.h"
#include "util.h"
#include "Instrumentor.h"

extern int rands;
extern int fast;
extern int point_gap;
extern int w_val;

static int wrap(int val, int max){
    val = val<0 ? val+max : val;
    val = val>=max ? val-max : val;
    return val;
}

void thruPair(int endPrior, int points_count, std::vector<nail_t> &nail, uint8_t *used, int w_val, int fast, int point_gap, const std::function<void(int,int,int)>& f){
    // PROFILE_FUNCTION();
    // TODO: point_gap musi być odpowiednie w stosunku do points_count
    int iEnd = w_val*2+1;
    for( int i=0;i<iEnd;i++ ){
        int start = wrap(i+endPrior-w_val,points_count);
        int jEnd = points_count-1-point_gap*2-i+(i<=point_gap?i:point_gap);
        for( int j=0;j<jEnd;j+=random(1,fast<=0?1:fast) ){
        // for( int j=0;j<jEnd;j+=1 ){
            int end = wrap(j+start+1+point_gap,points_count);
            // int end = wrap(random(0,points_count-1),points_count);
            // end = random(0,jEnd-1);
            // TODO: end = random

            if( nail.at(start).edge == nail.at(end).edge ) continue; // pomiń jeśli ta sama krawędź
            int usedIdx = getNailsIndex(start,end,points_count);
            if( used!=NULL)
            if( used[usedIdx]>0 ) continue; // pomiń nakładające się linie
            
            f(start,end,usedIdx);
        }
    }
}

void thruPairAll(int nailCnt, const std::function<void(int,int,int)>& f){
    std::vector<uint8_t> used(nailCnt*nailCnt,0);
    for( int start=0;start<nailCnt;start++ ){
        for( int end=0;end<nailCnt;end++ ){
            int usedIdx = getNailsIndex(start,end,nailCnt);
            if( used.at(usedIdx)>0 ) continue;
            if(start==end) continue;
            f(start,end,usedIdx);
            used.at(usedIdx) = 1;
        }
    }
}

volatile int randsTotal = 0;
void startEndNormalize(int &start, int &end, core_t &c){
    int nails = c.dst.nail.size();
    while(start<0){ start += nails; }
    while(end<0){ end += nails; }
    start = start%nails;
    end = end%nails;
    if(start>end){
        int startt = start;
        start = end;
        end = startt;
    }
}
void pairGenerator(core_t &c, int wireIdx, const std::function<void(int,int,int)>& f){
    if(rands>1){
        while(randsTotal>0)
        // for(int i=0;i<rands+1;i++)
        {
            randsTotal--;
            int start;
            int end;
            int usedIdx = -1;
            int timeout = 1000;
            int nails = c.dst.nail.size();
            {
                // PROFILE_SCOPE("rands");
                while(1){
                    if(--timeout==0) break;
                    start = random(0,nails-1);
                    end = random(0,nails-1);
                    startEndNormalize(start,end,c);
                    if(start!=end){
                    // if(abs(start-end)>=random(point_gap-5,point_gap+5)){
                        usedIdx = getNailsIndex(start,end,nails); // TODO:?
                        if( c.used.data[usedIdx]>0 ) continue;
                        break;
                    }
                }
                if(timeout<=0) return;
            }
            f(start,end,usedIdx);
        }
    } else {
        int endPrior = c.dst.wire.at(wireIdx).nail;
        thruPair(endPrior,c.dst.nail.size(),c.dst.nail,c.used.data,w_val,fast,point_gap,f);
    }
}

void pairGeneratorNear(core_t &c, int startI, int endI, int bb
, const std::function<void(int,int,int)>& f){
    for(int start_=startI-bb;start_<=startI+bb;start_++){
        for(int end_=endI-bb;end_<=endI+bb;end_++){
            int start = start_;
            int end = end_;
            if(start==startI && end==endI) continue;
            if(start==end) continue;
            startEndNormalize(start,end,c);
            int usedIdx = getNailsIndex(start,end,c.dst.nail.size());
            if( c.used.data[usedIdx]>0 ) continue;
            f(start,end,usedIdx);
        }
    }
}

void pairGeneratorR(int start, int end, core_t &c, int wireIdx, const std::function<void(int,int,int)>& f){
    while(randsTotal>0)
    // for(int i=0;i<rands+1;i++)
    {
        randsTotal--;
        int start;
        int end;
        int usedIdx = -1;
        int timeout = 1000;
        {
            // PROFILE_SCOPE("rands");
            while(1){
                if(--timeout==0) break;
                start = random(0,c.dst.nail.size()-1);
                end = random(0,c.dst.nail.size()-1);
                if(abs(start-end)>=random(point_gap-5,point_gap+5)){
                    usedIdx = getNailsIndex(start,end,c.dst.nail.size()); // TODO:?
                    if( c.used.data[usedIdx]>0 ) continue;
                    break;
                }
            }
            if(timeout<=0) return;
        }
        f(start,end,usedIdx);
    }
}