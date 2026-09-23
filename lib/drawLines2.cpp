#include "util.h"
int dirF = 1;
int layerIdxSelector(int vDir, int rr, int lines){
    int layerIdx = 0;
    double trans = 0;
    switch(vDir){
        case 0:{ // random
            layerIdx = random(0,lines-1);
            // TODO: random distribution
            break;
        }
        case 10:{ // random
            double weightSum = 0;
            double contrib = trans;
            for(int i=0;i<lines;i++){
                weightSum += contrib;
                contrib *= trans;
            }
            double rng = randomf(0,weightSum);
            double weightSum2 = 0;
            double contrib2 = trans;
            for(int i=0;i<lines;i++){
                weightSum2 += contrib2;
                contrib2 *= trans;
                layerIdx = lines-1-i;
                if(weightSum2>=rng){
                    break;
                }
                
            }
            // printf("%i %f %f\n",layerIdx,weightSum,weightSum2);
            // TODO: random distribution
            break;
        }
        case 1:{ // backward
            layerIdx = (lines-1)-rr%lines;
            break;
        }
        case 2:{ // forward
            layerIdx = rr%lines;
            break;
        }
        case 3:{ // forward-backward
            if(rr%lines==0) dirF = !dirF;
            layerIdx = dirF ? (rr%lines) : ((lines-1)-rr%lines);
            break;
        }
    }
    return layerIdx;
}