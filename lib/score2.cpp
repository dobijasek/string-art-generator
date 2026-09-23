#include "score.h"
#include "util.h"
#include "CIEDE2000.h"
#include "score2_method.h"

extern int vtanh;
extern int v0;
extern int v1;
extern core_t srcc;
extern int vnorm;

CIEDE2000::LAB rgb2lab(double R, double G, double B){
  double r = R / 255;
  double g = G / 255;
  double b = B / 255;

  r = (r > 0.04045) ? pow((r + 0.055) / 1.055, 2.4) : r / 12.92;
  g = (g > 0.04045) ? pow((g + 0.055) / 1.055, 2.4) : g / 12.92;
  b = (b > 0.04045) ? pow((b + 0.055) / 1.055, 2.4) : b / 12.92;

  double x = (r * 0.4124 + g * 0.3576 + b * 0.1805) / 0.95047;
  double y = (r * 0.2126 + g * 0.7152 + b * 0.0722) / 1.00000;
  double z = (r * 0.0193 + g * 0.1192 + b * 0.9505) / 1.08883;

  x = (x > 0.008856) ? pow(x, 1.0/3.0) : (7.787 * x) + 16.0/116.0;
  y = (y > 0.008856) ? pow(y, 1.0/3.0) : (7.787 * y) + 16.0/116.0;
  z = (z > 0.008856) ? pow(z, 1.0/3.0) : (7.787 * z) + 16.0/116.0;

  return {(116 * y) - 16, 500 * (x - y), 200 * (y - z)};
}

CIEDE2000::LAB gs2lab(double gs){
    return rgb2lab(gs,gs,gs);
}

double pscore2(double a, double b){
    CIEDE2000::LAB lab1 = gs2lab(a);
    CIEDE2000::LAB lab2 = gs2lab(b);
    // printf("%f   %f %f %f\n",a,lab1.l,lab1.a,lab1.b);
    // printf("%f   %f %f %f\n\n",b,lab2.l,lab2.a,lab2.b);
    return CIEDE2000::CIEDE2000(lab1, lab2);
}

extern int vcolorspace;
// inline scoreResult_t getPixelScore(int imgOffset,uint8_t color, double intensity, int layerIdxDraw, int layerIdxErase){
//     double div = (double)vnorm;
//     auto blend = srcc.dst.comp.at(imgOffset).dataTest(color,intensity,layerIdxDraw,layerIdxErase);
//     float cs = blend.cs;
//     float cdE = blend.cdErase;
//     float cdI = blend.cdInsert;
//     double errorE = fabs(cs - cdE);
//     double errorI = fabs(cs - cdI);
//     errorE = errorE/div;
//     errorI = errorI/div;
//     // dscore = vtanh<=0 ? dscore : tanh(dscore/(double)(vtanh));
//     errorE = tanh(mapf(errorE,0,vtanh,0,3));
//     errorI = tanh(mapf(errorI,0,vtanh,0,3));
//     errorE = pow(errorE,powexp);
//     errorI = pow(errorI,powexp);
//     if(vcolorspace==0){
//         errorE = pscore(cs/div,cdE/div,powexp);
//         errorI = pscore(cs/div,cdI/div,powexp);
//     } else {
//         errorE = pscore2(cs/div,cdE/div);
//         errorI = pscore2(cs/div,cdI/div);
//     }
//     // dscore = vtanh<=0 ? dscore : tanh(dscore/(double)(vtanh));
//     // errorE = tanh(mapf(errorE,0,vtanh,0,3));
//     // errorI = tanh(mapf(errorI,0,vtanh,0,3));
//     double dscore = errorI-errorE;
//     double dprocessed = blend.contribI;
//     // dscore = tanh(mapf(dscore,0,vtanh,0,3));
//     return {dscore, dprocessed, blend.contribI, blend.contribE, 1, errorE,errorI, cs, cdE, cdI};
// }

// inline outData_t getPixelData(int imgOffset,uint8_t color, double intensity, int layerIdxDraw, int layerIdxErase){
//     auto blend = srcc.dst.comp.at(imgOffset).dataTest(color,intensity,layerIdxDraw,layerIdxErase);
//     return blend;
// }

int scoreMethod = 0;
extern int vforceScoreMethod;
extern int vsamplingRate;
extern double powexp1;
extern double powexp2;
extern double powexp3;
scoreParam_t sm1;
scoreParam_t sm2;
scoreParam_t sm3;
extern int vclc;
#include "benchmark.h"
// extern benchmark_t b1,b2,b3,b4;
extern int B1,B2,B3,B4;
extern std::mutex Bmutex;
std::vector<scoreResult_t> getLineScore2(const std::vector<double> &color
,line_c &line, int layerIdxDraw, int layerIdxErase){
    benchmark_t b1,b2,b3,b4;
    std::vector<scoreResult_t> sr(color.size());
    auto lpx = line.pixelLoad();
    if(lpx.size()==0){
        for(auto &q : sr){
            q.score  = 0;
            q.score2 = 0;
            q.score3 = 0;
            q.score4 = 0;
            q.layerIdxDraw = layerIdxDraw;
            q.layerIdxErase = layerIdxErase;
        }
        // printf("zero size start:%i end:%i\n",line.s,line.e);
        return sr;
    }
    std::vector<std::vector<outData_t>> array_(color.size(), std::vector<outData_t>(lpx.size()));
    b1.start();
    int ii = 0;
    for(auto &pt : lpx){
        int imgOffset = pt.imgOffset;
        if(random(0,100)>vsamplingRate) continue;
        // auto val = getPixelData(imgOffset,color[colorIdx],pt.intensity,layerIdxDraw,layerIdxErase);
        auto val = srcc.dst.comp.at(imgOffset).dataTestv3(color,pt.intensity,layerIdxDraw,layerIdxErase);
        for(int colorIdx=0;colorIdx<color.size();colorIdx++){
            array_.at(colorIdx).at(ii) = val.at(colorIdx);
            val.at(colorIdx).offset = imgOffset;
        }
        ii++;
    }
    b1.stop();
    b2.start();
    
    for(int i=0;i<color.size();i++){
        auto array = array_.at(i);
        double scoreee  = -999999;
        double scoreee2 = -999999;
        double scoreee3 = -999999;
        scoreMethod = vforceScoreMethod>0 ? vforceScoreMethod : scoreMethod;
        scoreMethod = 1;
        switch(scoreMethod){
            case 1:
            {
                // scoreee = sm1.method==0 ? score_sum(array,sm1.powexp,sm1.normalize) : score_sumWeighted(array,sm1.powexp,sm1.normalize);
                // scoreee2 = sm2.method==0 ? score_sum(array,sm2.powexp,sm2.normalize) : score_sumWeighted(array,sm2.powexp,sm2.normalize);
                // scoreee3 = sm3.method==0 ? score_sum(array,sm3.powexp,sm3.normalize) : score_sumWeighted(array,sm3.powexp,sm3.normalize);
                // powexp = 3;
                // scoreee = -score_average(array);
                // scoreee2 = 0;
                // powexp = 9;
                // scoreee3 = score_sumWeighted(array,powexp3,0);
                // scoreee3 = 0;
                // scoreee = -score_averagePercentilCotrib2(array);
                // scoreee2 = score_sumWeighted(array,powexp,1);
                // scoreee2 = score_sum(array,powexp,0);
                // scoreee3 = score_contrib(array,powexp,1);
                // scoreee3 = score_averagePercentilCotrib(array);
                // scoreee2 = score_sumWeighted(array,powexp,1);
                // scoreee = score_gaussWindow(array,3);
                // scoreee2 = scoreee;
                // scoreee3 = score_gaussWindow(array,5);
                // scoreee3 = score_gaussWindow(array,9);
                // double m = scoreee + scoreee2 + scoreee3;
                // scoreee = sqrt(scoreee*scoreee + scoreee2*scoreee2 + scoreee3*scoreee3)*sign(m/3);
                // extern Paerson__ ps__;
                // scoreee = score_paerson(array,powexp,0)-ps__.compute();
                scoreee = -score_sum(array,powexp,0);
                // scoreee = score_paerson(array,powexp,0);
                // scoreee /= array.size();
                sr[i].pearson = score_paerson2(array);
                sr[i].pearson.N = array.size();
                scoreee2 = sr[i].pearson.compute5();
                // sr[i].pearson.print();
                // scoreee3 = score_countPeak(array);
                extern double qmean2;
                // scoreee2 = score_paerson(array,powexp,0)-ps__.compute();
                // scoreee2 *= array.size();
                // if(scoreee2>1){
                //     // ps__.print();
                // }
                // scoreee2 = -score_countDense(array);
                // scoreee2 = -score_sumTop(array);
                // scoreee2 = score_countPeakCont(array);
                scoreee3 = score_countPolarity(array);
                sr[i]._emptys = score_countEmpty(array);
                // scoreee2 = -score_countPeak(array);
                // scoreee2 = fabs(score_sum(array,1,1)-score_sum(array,powexp,1));
                // scoreee2 = fabs(score_sumWeighted(array,1,1)-score_sumWeighted(array,powexp,1));
                // scoreee2 = -score_sumWeighted(array,powexp,1);
                extern int vPercentil;
                // scoreee2 = score_sumPercetinl(array,0);
                // scoreee = -score_sumPercetinl(array,(double)vPercentil/100.0);
                // scoreee3 = scoreee;
                // scoreee = -score_stddev(array);
                // scoreee2 = -score_sum(array,powexp,0);
                // scoreee2 = scoreee;
                // printf("%f\n",scoreee);
                if(scoreee==0 || scoreee2==0){
                    // printf("%f %f %i %i\n",scoreee,scoreee2,array.size(),lpx.size());
                }
                break;
            }
            case 2:
            {
                scoreee = -score_sum(array,powexp,0);
                scoreee2 = score_paerson(array,powexp,0);
                scoreee3 = scoreee;
                // scoreee = score_sumWeighted(array,powexp,1);
                // scoreee2 = score_countColorContinousPeak(array);
                // scoreee3 = score_countPeak(array);
                break;
            }
            case 3:
            {
                scoreee = score_weightedAverage(array);
                break;
            }
            case 4:
            {
                double s1 = score_sumPercetinl(array,0);
                // double s1 = score_stddev(array);
                // double s2 = score_average(array);

                // scoreee = sqrt(s1*s1+s2*s2);
                scoreee = s1;
                break;
            }
            
        }
        sr[i].empty = 0;
        // double q1 = 1.0-tanh(mapf(scoreee2,0,vclc,0,3));
        // double q2 = 1.0-tanh(mapf(scoreee3,0,vclc,0,3));
        sr[i].score = scoreee;
        sr[i].score2 = scoreee2;
        sr[i].score3 = scoreee3;
        sr[i].layerIdxDraw = layerIdxDraw;
        sr[i].layerIdxErase = layerIdxErase;
        sr[i].len = line.pixelLoad().size();
        // sr[i].len = array_.at(i).size();
        // if(sr[i].len==0){
        //     sr[i].score  = DBL_MIN;
        //     // sr[i].score2 = DBL_MIN;
        //     sr[i].score3 = DBL_MAX;
        //     // printf("no len %i\n",scoreee);
        // }
    }
    b2.stop();
    if(array_.at(0).size()>100){
        Bmutex.lock();
        B1 = b1.nano()/array_.at(0).size();
        B2 = b2.nano()/array_.at(0).size();
        Bmutex.unlock();
    }
    return sr;
}

std::vector<scoreResult_t> getLineScoreqq(const std::vector<double> &color, int start, int end
    , int layerIdxDraw, int layerIdxErase){
    auto line = srcc.getLine(start,end);
    return getLineScore2(color,line,layerIdxDraw,layerIdxErase);
}