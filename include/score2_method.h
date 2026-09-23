#pragma once
#include <vector>
#include "score.h"
#include "layer.h"

extern float powexp;
extern Paerson__ ps__;
double score_sum(const std::vector<outData_t> &array, double pe,int normalize){
    if(array.size()<=7) return DBL_MAX;
    double sumE = 0;
    double sumI = 0;
    double sumD = 0;
    double sum = 0;
    // cs = 10;
    // cdi = 200;
    // cde = 180;
    for(auto &v : array){
        double E = pscore(v.cs,v.cdErase,pe);
        double I = pscore(v.cs,v.cdInsert,pe);
        sumE += E; // abs(180-10)=170
        sumI += I; // abs(200-10)=190
        // sumD += 255-fabs(E-I); // abs(170-190)=20
    }
    // int windowSize = 5;
    // int cnt = 0;
    // for(int i=windowSize;i<array.size()-windowSize;i++){
    //     for(int j=-windowSize;j<windowSize+1;j++){
    //         if(j==0) continue;
    //         auto &v = array.at(i);
    //         auto &p = array.at(i+j);
    //         double E = pscore((p.cs-v.cs), (p.cdErase  - v.cdErase ) ,pe);
    //         double I = pscore((p.cs-v.cs), (p.cdInsert - v.cdInsert) ,pe);
    //         sumE += E; // abs(180-10)=170
    //         sumI += I; // abs(200-10)=190
    //         cnt+=1;
    //     }
    // }
    double score = (sumI-sumE)/255; // 190-170-20=10
    // score = sumI;
    // score /= cnt;
    // score /= array.size();
    // score /= sw;
    if(normalize){
    }
    // score = pow(fabs(score),1.0/pe)*sign(score);
    return score;
}

double score_sumTop(const std::vector<outData_t> &array){
    double sum = 0;
    double cnt = 0;
    double div = 255;
    for(auto &v : array){
        if(v.contribI!=1) continue;
        sum += fabs(v.cs-v.cdInsert);
        cnt += 1;
    }
    double score = cnt>0 ? sum/cnt : 0;
    return score;
}

double score_paerson(const std::vector<outData_t> &array, double pedfg,int normalize){
    Paerson__ ps__c = ps__;
    if(array.size()==0){
        printf("zero len pearson\n");
        return -1;
    }
    for(auto &v : array){
        ps__c.update(v.offset,v.cs,v.cdErase,v.cs,v.cdInsert,v.weight,v.weight);

    }
    double score = ps__c.qq();
    return score;
}
#include "paerson.h"
Paerson__ score_paerson2(const std::vector<outData_t> &array){
    Paerson__ pae;
    for(auto &v : array){
        pae.update(v.offset,v.cs,v.cdErase,v.cs,v.cdInsert,v.weight,v.weight);
        // pae.update(v.offset,0,fabs(v.cs-v.cdErase),0,fabs(v.cs-v.cdInsert),v.weight,v.weight);
    }
    pae.SW = array.size();
    return pae;
}

Paerson__ score_paerson22(const std::vector<outData_t> &array){
    Paerson__ pae;
    for(auto &v : array){
        pae.add(v.offset,v.cs,v.cdErase,1);
        pae.update(v.offset,v.cs,v.cdErase,v.cs,v.cdInsert,v.weight,v.weight);
    }
    return pae;
}

double score_countPeak(const std::vector<outData_t> &array){
    if(array.size()==0){
        printf("zero len peak\n");
        return 100;
    }
    int cntMax = 0;
    for(auto &v : array){
        int tmp = v.contribI==1;
        // printf("%i ,",tmp);
        cntMax += tmp;
    }
    // return cntMax;
    if(cntMax>array.size()){
        // printf("\n%i %i\n",cntMax,array.size());
    }
    return cntMax;
}

double score_countEmpty(const std::vector<outData_t> &array){
    if(array.size()==0){
        printf("zero len peak\n");
        return 100;
    }
    int cntMax = 0;
    for(auto &v : array){
        int tmp = v.contribE==1;
        cntMax += tmp;
    }
    return cntMax;
}

double score_countDense(const std::vector<outData_t> &array){
    if(array.size()==0){
        printf("zero len peak\n");
        return 100;
    }
    int cntMax = 0;
    for(auto &v : array){
        cntMax = v.contribE>cntMax&&v.contribI==1 ? v.contribE : cntMax;
    }
    return cntMax;
}

double score_countPeakCont(const std::vector<outData_t> &array){
    // if(array.size()==0){
    //     printf("zero len peak\n");
    //     return 10000;
    // }
    int peakCont = 0;
    int peakContMax = 0;
    int q = 0;
    int pq = 0;
    for(auto &v : array){
        q = v.contribI==1 ? 1 : 0;
        if(q==1){
            if(pq==0){
                peakCont = 0;
            }
            peakCont += 1;
            pq = 1;
        }
        if(q==0){
            pq = 0;
        }
        peakContMax = peakCont>peakContMax ? peakCont : peakContMax;
    }
    return peakContMax;
}

double score_countPolarity(const std::vector<outData_t> &array){
    if(array.size()==0){
        printf("zero len peak\n");
        return 10000;
    }
    int signChanges = 0;
    int signp = -1;
    for(auto &v : array){
        int sign = (v.cs-v.cdInsert)>=0;
        if(sign!=signp){
            signp = sign;
            signChanges += 1;
        }
    }
    return (double)signChanges/(double)array.size();
    // return signChanges;
}

double score_sumx(const std::vector<outData_t> &array, double pe,int normalize){
    if(array.size()==0) return DBL_MAX;
    double sumE = 0;
    double sumI = 0;
    double sum = 0;
    double div = 1;
    double weights = 0;
    double minI = DBL_MAX;
    double maxI = DBL_MIN;
    double minE = DBL_MAX;
    double maxE = DBL_MIN;
    for(auto &v : array){
        double E = pscore(v.cs/div,v.cdErase/div,pe);
        double I = pscore(v.cs/div,v.cdInsert/div,pe);
        minI = I<minI ? I : minI;
        maxI = I>maxI ? I : maxI;
        minE = E<minE ? E : minE;
        maxE = E>maxE ? E : maxE;
    }
    // score = pow(fabs(score),1.0/pe)*sign(score);
    return fabs(maxI-minI)-fabs(maxE-minE);
}

double score_softmax(const std::vector<outData_t> &array, double pe,int normalize){
    if(array.size()==0) return DBL_MAX;
    double sumE = 0;
    double sumI = 0;
    double sum = 0;
    double div = 255;
    double weights = 0;
    double score = (sumI-sumE);
    for(auto &v : array){
        sumE += exp(pscore(v.cs/div,v.cdErase/div,pe));
        sumI += exp(pscore(v.cs/div,v.cdInsert/div,pe));
    }
    // sumE /= array.size();
    // sumI /= array.size();
    for(auto &v : array){
        // double q = 0;
        double i = (exp(pscore(v.cs/div,v.cdInsert/div,pe))/sumI);
        double e = (exp(pscore(v.cs/div,v.cdErase/div,pe))/sumE);
        // q = q*q;
        printf("%f %f\n",i,e);
        double q = 0.25;
        if(i>q || e>q)
        {
            score += +pscore(v.cs/div,v.cdInsert/div,pe);
            score += -pscore(v.cs/div,v.cdErase/div,pe);
            weights += 1;
        }
    }
    if(weights!=0) score /= weights;
    if(normalize){
        // score /= array.size();
    }
    // score = pow(fabs(score),1.0/pe)*sign(score);
    return score;
}

double score_dsum(const std::vector<outData_t> &array, double pe,int normalize){
    if(array.size()==0) return DBL_MAX;
    double sumE = 0;
    double sumI = 0;
    double sum = 0;
    double div = 255;
    double weights = 0;
    double pvs = 0;
    double pve = 0;
    double pvi = 0;
    double scoree = 0;
    for(auto &v : array){
        double w = v.contribI;
        w = 1;
        weights += w;
        double dvs = (v.cs-pvs);
        double dve = (v.cdErase-pve);
        double dvi = (v.cdInsert-pvi);
        // sumE += pscore(dvs/div,dve/div,pe)*w;
        // sumI += pscore(dvs/div,dvi/div,pe)*w;
        // sumE = dvs-dve;
        // sumI = dvs-dvi;
        sumE +=  pscore(v.cs/div,v.cdErase/div,pe)*(dve)*w;
        sumI += pscore(v.cs/div,v.cdInsert/div,pe)*(dvi)*w;
        // scoree += - pscore(v.cs/div,v.cdInsert/div,pe);
        scoree += - pscore(v.cs/div,v.cdErase/div,pe);
        scoree += + pscore(v.cdInsert/div,v.cdErase/div,pe);
        // scoree -= pscore(v.cs/div,v.cdErase/div,pe)*pscore(v.cs/div,v.cdInsert/div,pe);
        pvs = v.cs;
        pve = v.cdErase;
        pvi = v.cdInsert;
    }
    double score = (sumI-sumE);
    if(normalize){
        // score /= weights;
    }
    // score = pow(fabs(score),1.0/pe)*sign(score);
    return scoree;
}
extern int dLPF;
extern double gmin,gmax;
double score_sumWeighted(const std::vector<outData_t> &array, double pe, int normalize){
    if(array.size()==0) return DBL_MAX;
    double sumE = 0;
    double sumI = 0;
    double sum = 0;
    double div = 1;
    double weights = 0;
    double weightsI = 0;
    double weightsE = 0;
    for(auto &v : array){
        // double we = pow(v.weight,15);

        // double qqq = fabs(v.cdInsert-v.cs);
        // qqq = mapf(qqq,0,vtanh,0,3);
        // qqq = tanh(qqq);
        // double p = mapf(dLPF,0,1000,0.1,10);
        // qqq = pow(qqq,p);
        // qqq = mapf(qqq,gmin,gmax,0,1);

        // double www = fabs(v.cdErase-v.cs);
        // www = mapf(www,0,vtanh,0,3);
        // www = tanh(www);
        // // double p = mapf(dLPF,0,1000,0.1,10);
        // www = pow(www,p);
        // www = mapf(www,gmin,gmax,0,1);

        // if(weightsI==0) printf("%f %f\n",qqq,v.weight);

        double wi = v.weight;
        double we = v.weight;
        // wi = qqq;
        // we = www;
        // wi *= v.contribI;
        // we *= v.contribE;
        weightsI += wi;
        weightsE += we;
        sumI += pscore(v.cs/div,v.cdInsert/div,pe)*wi;
        sumE += pscore(v.cs/div,v.cdErase/div,pe)*we;
    }
    if(normalize){
        sumI /= weightsI;
        sumE /= weightsE;
    }
    double score = sumI-sumE;
    // double score = pow(sumI,1.0/powexp)-pow(sumE,1.0/powexp);
    // score = pow(fabs(score),1.0/powexp)*sign(score);
    return score;
}



double score_countColorContinousPeak(const std::vector<outData_t> &array){
    if(array.size()==0) return DBL_MAX;
    double cnt = 0;
    double cntMax = DBL_MIN;
    for(auto &v : array){
        // if(v.contribI<=1){
        if(fabs(v.cs-v.cdInsert)>30){
        //     cnt += 1;
            // double a = 1.0-tanh(v.contribI);
            double a = 1;
            cnt += a;
            cntMax = std::max(cntMax,cnt);
        } else {
            cntMax = std::max(cntMax,cnt);
            cnt = 0;
            // cnt = 1.0-tanh(v.contribI);
        }
        // cntMax = std::max(cntMax,cnt);
    }
    return cntMax;
}

double score_contrib(const std::vector<outData_t> &array, double pe, int normalize){
    if(array.size()==0) return DBL_MAX;
    double score;
    double weights = 0;
    for(auto &v : array){
        weights += v.weight;
        score += v.contribI*v.weight;
    }
    if(normalize){
        score /= weights;
    }
    // score = pow(fabs(score),1.0/powexp)*sign(score);
    return score;
}

double score_sum2(const std::vector<outData_t> &array){
    double sumE = 0;
    double sumI = 0;
    double sum = 0;
    double div = 255;
    double weights = 0;
    for(auto &v : array){
        double we = pow(v.weight,6);
        // we = 1;
        weights += we;
        sumE += pscore(v.cs/div,v.cdErase/div,powexp)*we;
        sumI += pscore(v.cs/div,v.cdInsert/div,powexp)*we;
        // printf("%f\n",v.weight);
        // sumE += pscore(v.cs,v.cdErase,powexp*3);
        // sumI += pscore(v.cs,v.cdInsert,powexp*3);
        // sumE += pscore(v.cs,v.cdErase,powexp*4);
        // sumI += pscore(v.cs,v.cdInsert,powexp*4);
        // sumE += pscore(v.cs,v.cdErase,powexp*5);
        // sumI += pscore(v.cs,v.cdInsert,powexp*5);
        // sum += pscore(v.cdErase,v.cdInsert,powexp);
    }
    double score = sumI-sumE;
    score = pow(fabs(score),1.0/powexp)*sign(score);
    return score;
}

double score_sumPercetinl(const std::vector<outData_t> &array, double percentil){
    double sumE = 0;
    double sumI = 0;
    double sum = 0;
    double div = 255;
    double minE = DBL_MAX;
    double maxE = DBL_MIN;
    double minI = DBL_MAX;
    double maxI = DBL_MIN;
    double min = DBL_MAX;
    double max = DBL_MIN;
    for(auto &v : array){
        double wi = v.weight;
        double we = v.weight;
        double E = pscore(v.cs/div,v.cdErase/div,powexp)*we;
        double I = pscore(v.cs/div,v.cdInsert/div,powexp)*wi;
        min = std::min(fabs(I-E),min);
        max = std::max(fabs(I-E),max);
        minI = std::min(I,minI);
        maxI = std::max(I,maxI);
        minE = std::min(E,minE);
        maxE = std::max(E,maxE);
    }
    
    double q = 0.25;
    q = percentil;
    // q = randomf(0,0.15);
    double a = minE + q*(maxE-minE);
    double b = minI + q*(maxI-minI);
    // a = ;
    double c = min + q*(max-min);
    int cnt = 0;
    double score = 0;
    double weightsE = 0;
    double weightsI = 0;
    for(auto &v : array){
        // if(v.contribI!=1) continue;
        double wi = v.weight;
        double we = v.weight;
        double E = pscore(v.cs/div,v.cdErase/div,powexp)*we;
        double I = pscore(v.cs/div,v.cdInsert/div,powexp)*wi;
        double s = I-E;
        // if(E<a && I<b) continue;
        // if(!(E>a || I>b)) continue;
        // if(!(E>a && I>b)) continue;
        // if(fabs(s)<c) continue;
        // score += I-E;
        
        if(E>a){
            sumE += E;
            weightsE += we;
        }
        if(I>b){
            sumI += I;
            weightsI += wi;
        }
        // cnt++;
    }
    // score = sumI-sumE;
    // if(cnt>0){
    if(weightsI!=0 && weightsE!=0){
        score = sumI/weightsI-sumE/weightsE;
        // score /= cnt;
    }
    // score = pow(fabs(score),1.0/powexp)*sign(score);
    return score;
}

double score_sumPercetinlW(const std::vector<outData_t> &array, double percentil){
    double sumE = 0;
    double sumI = 0;
    double sum = 0;
    double div = 255;
    double minE = DBL_MAX;
    double maxE = DBL_MIN;
    double minI = DBL_MAX;
    double maxI = DBL_MIN;
    double min = DBL_MAX;
    double max = DBL_MIN;
    for(auto &v : array){
        double wi = v.weight;
        double we = v.weight;
        double E = we;
        double I = wi;
        min = std::min(fabs(I-E),min);
        max = std::max(fabs(I-E),max);
        minI = std::min(I,minI);
        maxI = std::max(I,maxI);
        minE = std::min(E,minE);
        maxE = std::max(E,maxE);
    }
    
    double q = 0.25;
    q = percentil;
    // q = randomf(0,0.15);
    double a = minE + q*(maxE-minE);
    double b = minI + q*(maxI-minI);
    // a = ;
    double c = min + q*(max-min);
    int cnt = 0;
    double score = 0;
    double weightsE = 0;
    double weightsI = 0;
    for(auto &v : array){
        double wi = v.weight;
        double we = v.weight;
        double E = we;
        double I = wi;
        double s = I-E;
        // if(E<a && I<b) continue;
        // if(!(E>a || I>b)) continue;
        // if(!(E>a && I>b)) continue;
        // if(fabs(s)<c) continue;
        // score += I-E;
        
        if(E>a){
            sumE += E;
            // weightsE += we;
        }
        if(I>b){
            sumI += I;
            // weightsI += wi;
        }
        // cnt++;
    }
    score = sumI-sumE;
    // if(cnt>0){
    if(weightsI!=0 && weightsE!=0){
        // score = sumI/weightsI-sumE/weightsE;
        // score /= cnt;
    }
    // score = pow(fabs(score),1.0/powexp)*sign(score);
    return score;
}

double score_averagePercentilCotrib(const std::vector<outData_t> &array){
    double sum = 0;
    double min = DBL_MAX;
    double max = DBL_MIN;
    for(auto &v : array){
        min = std::min(v.contribI,min);
        max = std::max(v.contribI,max);
    }
    double q = 0.25;
    double a = min + q*(max-min);
    int cnt = 0;
    for(auto &v : array){
        double s = v.contribI;
        if(!(s<a)) continue;
        sum += v.contribI;
        cnt++;
    }
    double score = cnt>0 ? sum : DBL_MAX;
    return score;
}

double score_averagePercentilCotrib2(const std::vector<outData_t> &array){
    double sum = 0;
    double min = DBL_MAX;
    double max = DBL_MIN;
    double div = 255.0;
    double pe = 1;
    for(auto &v : array){
        min = std::min(v.contribI,min);
        max = std::max(v.contribI,max);
    }
    double q = 0.25;
    double a = min + q*(max-min);
    int cnt = 0;
    for(auto &v : array){
        double s = v.contribI;
        if(s<a) continue;
        double E = pscore(v.cs/div,v.cdErase/div,pe);
        double I = pscore(v.cs/div,v.cdInsert/div,pe);
        sum += I-E;
        cnt++;
    }
    double score = cnt>0 ? sum/cnt : DBL_MAX;
    return sum;
}

double score_average(const std::vector<outData_t> &array){
    double errorE = 0;
    double errorI = 0;
    for(auto &v : array){
        errorE += pscore(v.cs,v.cdErase,powexp);
        errorI += pscore(v.cs,v.cdInsert,powexp);
    }
    double score = errorI-errorE;
    return score/array.size();
}

double score_gaussWindow(const std::vector<outData_t> &array, int rozmiar_okna=9){
    double score = 0;
    int n = array.size(); // Długość listy pikseli
    // int rozmiar_okna = 9;
    double sigma = rozmiar_okna/5.0;
    double wagi = 0;
    for (int i = 0; i < n; ++i) {
        double suma_wazona_roznic = 0.0;
        double suma_wag = 0.0;
        double cs_sum = 0;
        double cde_sum = 0;
        double cdi_sum = 0;
        double con_sum = 0;
        double score_sum = 0;
        for (int j = -rozmiar_okna / 2; j <= rozmiar_okna / 2; ++j) {
            int index = i + j;
            // double waga = array.at(i).contribI;
            if (index >= 0 && index < n) {
                int odleglosc = j;
                double waga = exp(-(odleglosc*odleglosc) / (2*sigma*sigma)); // Waga Gaussowska
                cs_sum += array.at(i).cs*waga;
                cde_sum += array.at(i).cdErase*waga;
                cdi_sum += array.at(i).cdInsert*waga;
                con_sum += array.at(i).contribI*waga;
                score_sum += pscore(array.at(i).cs,array.at(i).cdInsert,powexp)*waga;
                score_sum -= pscore(array.at(i).cs,array.at(i).cdErase,powexp)*waga;
                suma_wag += waga;
            }
        }
        // double errorE = pscore(cs_sum/suma_wag,cde_sum/suma_wag,powexp);
        // double errorI = pscore(cs_sum/suma_wag,cdi_sum/suma_wag,powexp);
        // double srednia_roznica_wazona = suma_wazona_roznic/suma_wag;
        // double ww = con_sum/suma_wag;
        // ww = 1;
        // score += (errorI-errorE)*ww;
        score += score_sum/suma_wag;
        // wagi += ww;
    }
    return score;
}

double score_weightedAverage(const std::vector<outData_t> &array){
    double sum = 0;
    double contrib = 0;
    for(auto &v : array){
        double errorE = pscore(v.cs,v.cdErase,powexp);
        double errorI = pscore(v.cs,v.cdInsert,powexp);
        double error = errorI-errorE;
        sum += error * v.contribI;
        contrib += v.contribI;
    }
    return sum/contrib;
}

double score_weightedAverage2(const std::vector<outData_t> &array){
    double sumE = 0;
    double sumI = 0;
    double contribE = 0;
    double contribI = 0;
    for(auto &v : array){
        sumE += pscore(v.cs,v.cdErase,powexp)*v.contribE;
        sumI += pscore(v.cs,v.cdInsert,powexp)*v.contribI;
        contribE += v.contribE;
        contribI += v.contribI;
    }
    return sumI/contribI - sumE/contribE;
}

double score_weightedAverage2(const std::vector<scoreResult_t> &array){
    double sum = 0;
    double contrib = 0;
    for(auto &v : array){
        sum += v.score*v.contribI*v.contribE;
        contrib += v.contribI*v.contribE;
    }
    return sum/contrib;
}

double score_custom2(const std::vector<scoreResult_t> &array){
    double sumE = 0;
    double sumI = 0;
    for(auto &v : array){
        sumE += v.scoreE;
        sumI += v.scoreI;
    }
    double meanE = sumE/array.size();
    double meanI = sumI/array.size();
    return meanI-meanE;
}

double score_custom3(const std::vector<scoreResult_t> &array){
    double sumE = 0;
    double sumI = 0;
    double sumCE = 0;
    double sumCI = 0;
    for(auto &v : array){
        sumE += v.scoreE*v.contribE;
        sumI += v.scoreI*v.contribI;
        sumCE += v.contribE;
        sumCI += v.contribI;
    }
    double score = sumI/sumCI-sumE/sumCE;
    // score /= sumC;
    return score;
}

double squareSigned(double val){
    return (val*val)*sign(val);
}
double score_stddev(const std::vector<outData_t> &array){
    double size = array.size();
    double sumE = 0;
    double sumI = 0;
    double errorE = 0;
    double errorI = 0;
    double weights = 0;
    for(auto &v : array){
        weights += v.weight;
        sumE += pscore(v.cs,v.cdErase,powexp)*v.weight;
        sumI += pscore(v.cs,v.cdInsert,powexp)*v.weight;
        // errorE += pscore(v.cs,v.cdErase,powexp);
        // errorI += pscore(v.cs,v.cdInsert,powexp);
    }
    double meanE = sumE/size;
    double meanI = sumI/size;
    double stddevE = 0;
    double stddevI = 0;
    for(auto &v : array){
        stddevE += square(pscore(v.cs,v.cdErase,powexp)*v.weight-meanE);
        stddevI += square(pscore(v.cs,v.cdInsert,powexp)*v.weight-meanI);
    }
    stddevE = sqrt(stddevE/size);
    stddevI = sqrt(stddevI/size);
    // stddevE = sqrt(stddevE*stddevE + meanE*meanE);
    // stddevI = sqrt(stddevI*stddevI + meanI*meanI);
    return (stddevI-stddevE);
}

double score_stddev2(const std::vector<scoreResult_t> &array){
    double size = array.size();
    double scSum = 0;
    double cdeSum = 0;
    double cdiSum = 0;
    for(auto &v : array){
        scSum += v.cs;
        cdeSum += v.cdE;
        cdiSum += v.cdI;
    }
    double csMean = scSum/size;
    double cdeMean = cdeSum/size;
    double cdiMean = cdiSum/size;
    double stddevE = 0;
    double stddevI = 0;
    for(auto &v : array){
        stddevE += square(fabs(csMean-v.cdE)-fabs(cdeMean-v.cs));
        stddevI += square(fabs(csMean-v.cdI)-fabs(cdiMean-v.cs));
        // stddevE = pscore(cdeMean,fabs(csMean-v.cs),powexp);
        // stddevI = pscore(cdiMean,v.cdI,powexp);
    }
    // stddevE = sqrt(cdeMean-csMean);
    // stddevI = sqrt(cdiMean-csMean);
    return (stddevI-stddevE);
}

// double score_stderr(std::vector<scoreResult_t> &array){
//     return score_stddev(array)/sqrt(array.size());
// }