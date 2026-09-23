#include "paerson.h"

#pragma once
#include <cmath>
#include "paerson2.h"
#include "util.h"

inline double pscorex(double s, double d,double q1){
  extern int vtanh;
  double diff = fabs(s-d);
  double diffScaled = mapf(diff,0,vtanh,0,2);
  double diffTanh = tanh(diffScaled);
  diff = diffTanh;
  // return diff;
  return pow(diff,q1);
}

void Paerson__::init(int w, int h){
    // mask.assign(w*h,1);
    // for(auto &q : mask){
    //   q = random(0,100)<=60;
    // }
}

void Paerson__::clear(){
    N   = 0;
    SA  = 0;
    SB  = 0;
    SW  = 0;
    SQ  = 0;
    SA2 = 0;
    SA3 = 0;
    SA4 = 0;
    SB2 = 0;
    SB3 = 0;
    SB4 = 0;
    SAB = 0;
    SDAB = 0;
    SDAB2 = 0;
}

void Paerson__::add(const Paerson__ &pa){
    SA  += pa.SA;
    SB  += pa.SB;
    SW  += pa.SW;
    SQ  += pa.SQ;
    SA2 += pa.SA2;
    SA3 += pa.SA3;
    SA4 += pa.SA4;
    SB2 += pa.SB2;
    SB3 += pa.SB3;
    SB4 += pa.SB4;
    SAB += pa.SAB;
    SDAB += pa.SDAB;
    SDAB2 += pa.SDAB2;
}

void Paerson__::sub(const Paerson__ &pa){
    SA  -= pa.SA;
    SB  -= pa.SB;
    SW  -= pa.SW;
    SQ  -= pa.SQ;
    SA2 -= pa.SA2;
    SA3 -= pa.SA3;
    SA4 -= pa.SA4;
    SB2 -= pa.SB2;
    SB3 -= pa.SB3;
    SB4 -= pa.SB4;
    SAB -= pa.SAB;
    SDAB -= pa.SDAB;
    SDAB2 -= pa.SDAB2;
}

void Paerson__::add(int idx, double A, double B,double W){
    update(idx,0,0,A,B,0,W);
    N++;
}
double pp = 4;
double dmean = 0;
double ddev = 1;
extern int vr1;
extern int vr2;
extern float powexpL;
void Paerson__::update(int idx, double oA, double oB, double nA, double nB,double oW, double nW){
    // if(mask.at(idx)==0) return;
    // if(nB==253){
    //     nA = 0;
    //     nB = 255;
    // }
    // if(oB==253){
    //     oA = 0;
    //     oB = 255;
    // }
    extern double srcMin,srcMax;
    // oB = mapf(oB,vr1,vr2,srcMin,srcMax);
    // nB = mapf(nB,vr1,vr2,srcMin,srcMax);
    SW  += nW - oW;
    SA  += nA*nW - oA*oW;
    SB  += nB*nW - oB*oW;
    // SW  += nW - oW;
    SQ  += pow(fabs(nA-nB)/255.0*nW,p) - pow(fabs(oA-oB)/255.0*oW,p);
    // SQ  += pscorex(nA,nB,p) - pscorex(oA,oB,p);
    SA2 += (nA*nA)*nW - (oA*oA)*oW;
    SA3 += nA*nA*nA - oA*oA*oA;
    SA4 += nA*nA*nA*nA - oA*oA*oA*oA;
    SB2 += (nB*nB)*nW - (oB*oB)*oW;
    SB3 += nB*nB*nB - oB*oB*oB;
    SB4 += nB*nB*nB*nB - oB*oB*oB*oB;
    SAB += (nA*nB)*nW - (oA*oB)*oW;
    double nd = (nA-nB);
    double od = (oA-oB);
    extern int vtanh;
    nd = fabs(nd);
    od = fabs(od);
    nd = (nd-dmean)/ddev;
    od = (od-dmean)/ddev;
    // nd = mapf(nd,0,vtanh,0,1);
    // od = mapf(od,0,vtanh,0,1);
    // nd = tanh(nd);
    // od = tanh(od);
    // nd = sign(nd)*pow(fabs(nd),powexpL);
    // od = sign(od)*pow(fabs(od),powexpL);
    SDAB  += pow(nd*nW,1) - pow(od*oW,1);
    SDAB2 += pow(nd*nW,2) - pow(od*oW,2);
}


double Paerson__::qq(){
    // return compute3();
    double cnt = SW;
    cnt = N;
    // return -sqrt(pow(SDAB2/cnt,1) - pow(SDAB/cnt,4));
    double score =  -pow(pow(SDAB2/cnt,1) - pow(SDAB/cnt,2),1.0/2.0);
    // score = -pow(-score,powexpL);
    return score;
    // double num = N*SAB - SA*SB;
    // double den = sqrt((N*SA2 - SA*SA)*(N*SB2 - SB*SB));
    // return den != 0 ? num/den : 0.0;

    // double den = sqrt(SA2 * SB2);
    // return den != 0 ? -sqrt((SDAB2/cnt) - (SDAB/cnt) * (SDAB/cnt))*SAB/den : 0.0;
}

double Paerson__::qq2(){

}

void Paerson__::print(){
    printf("%i %f %f %f %f %f\n\n",N,SA,SB,SA2,SB2,SAB);
}

double Paerson__::compute(){
    return -sqrt((SB2/N) - (SB/N) * (SB/N));
    // return 1-pow(SQ/N,1.0/p);
}

double Paerson__::compute4(){ // pearson
    double num = N*SAB - SA*SB;
    double den = sqrt((N*SA2 - SA*SA)*(N*SB2 - SB*SB));
    double score = den!=0 ? num/den : DBL_MIN;
    return score;
}

double Paerson__::compute5(){ // pearson
    double den = sqrt(SA2 * SB2);
    return den != 0 ? SAB / den : 0.0;

    // return N > 0 ? ((SAB / N) - (SA / N) * (SB / N)) : 0.0;;
}

double Paerson__::compute3() const {
    double qq = 0;
    {
        double den = pow(SA2 * SB2,1.0/2.0);
        qq = den != 0 ? SAB/den : 0.0;
        return qq;
    }

    double range = 1.0;
    double C1=(0.01*range)*(0.01*range);
    double C2=(0.03*range)*(0.03*range);
    double winArea = N;

    double muA = SA / winArea;
    double muB = SB / winArea;

    double sigmaA2 = SA2 / winArea - muA*muA;
    double sigmaB2 = SB2 / winArea - muB*muB;
    double sigmaAB = SAB / winArea - muA*muB;

    double num = (2*muA*muB + C1) * (2*sigmaAB + C2);
    double den = (muA*muA + muB*muB + C1) * (sigmaA2 + sigmaB2 + C2);
    return num/den*qq;
}

double Paerson__::compute2(){
    // Standardowe stałe SSIM – możesz zmienić, jeśli używasz zakresu 0–1:
    const double C1 = (0.01 * 255) * (0.01 * 255);
    const double C2 = (0.03 * 255) * (0.03 * 255);

    const double winArea = N;  // dla Twojej wersji BEZ okien

    // ŚREDNIE
    double muA = SA / winArea;
    double muB = SB / winArea;

    // WARIANCJE I KOWARIANCJA
    double sigmaA2 = SA2 / winArea - muA * muA;
    double sigmaB2 = SB2 / winArea - muB * muB;
    double sigmaAB = SAB / winArea - muA * muB;

    // Zapobiegamy nan
    double stdA = sqrt(std::max(0.0, sigmaA2));
    double stdB = sqrt(std::max(0.0, sigmaB2));

    double denomStd = stdA * stdB + 1e-12;

    // --- LUMINANCE ---
    double L = (2 * muA * muB + C1) /
                (muA * muA + muB * muB + C1);

    // --- CONTRAST (umowne rozbicie czynnika CS) ---
    double C = (2 * stdA * stdB + C2) /
                (sigmaA2 + sigmaB2 + C2 + 1e-12);

    // --- STRUCTURE (umowne rozbicie czynnika CS) ---
    double s_ab = sigmaAB / denomStd;  // "czysta korelacja"
    double S  = (s_ab + C2 / (2 * denomStd)) /
                (1.0  + C2 / (2 * denomStd));

    // --- FINALNY SSIM ---
    double SSIM = L*C*S;

    return SSIM;
}