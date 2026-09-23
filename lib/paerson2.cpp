#include "paerson2.h"
#include <cmath>
#include "util.h"
#include "metricX.h"

int Paerson__2::idx(int x, int y){
    if(x<0 || y<0 || x>=w || y>=h) return 0;
    return y*w+x;
}

void Paerson__2::init(int w_, int h_){
    w = w_;
    h = h_;
    r = 1;
    A.assign(w*h,0);
    B.assign(w*h,0);
    Q.resize(w*h);
    for(int i=0;i<500;i++){
        int x0 = random(0+r,w-1-r);
        int y0 = random(0+r,h-1-r);
        auto &q = Q[idx(x0,y0)];
        q.clear();
        q.reserve((r+1)*(r+1));
        for(int y=-r;y<r;y++)
            for(int x=-r;x<r;x++){
                int ii = idx(x0+x,y0+y);
                q.push_back({&A.at(ii),&B.at(ii)});
        }
    }
}

void Paerson__2::clear(){
}

void Paerson__2::add(int idx, double A, double B,double W){
    update(idx,0,0,A,B,0,W);
}

void Paerson__2::update(int idx, double oA, double oB, double nA, double nB,double oW, double nW){
    A[idx] = nA;
    B[idx] = nB;
}

double Paerson__2::compute(){
    double sumS = 0;
    double sumN = 0;
    for(auto &q : Q){
        if(q.size()==0) continue;
        metricX m;
        for(auto &p : q){
            double a = *p.A;
            double b = *p.B;
            m.add(a,b,1);
        }
        double score = m.pearson();
        sumS += score;
        sumN += 1;
    }
    return sumS/sumN;
}