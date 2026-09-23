#include "composite.h"

void composite_t::init(cv::Mat src, cv::Mat WEIGHT){
    raw64 = src.clone();
    raw64.convertTo(raw64,CV_64F,1,0);
    src64 = raw64.clone();
    rng = raw64.clone();
    w = raw64.cols;
    h = raw64.rows;
    layerq.clear();
    double *sp = (double*)raw64.data;
    double background = 253;
    for(int i=0;i<w*h;i++){
        double *pweight = &WEIGHT.at<double>(i);
        auto cp = computePixel_t(sp[i],background,pweight);
        layerq.push_back(cp);
    }
}

void composite_t::clear(){
    layerq.clear();
    lil.clear();
}

bool composite_t::validateLst(const char *s,int layerIdx){
    int i = 0;
    // for(auto it=lil.begin();it!=lil.end();it++){
    //     printf("%i\n",*it);
    // }
    if(lil.begin()->layerIdx!=0) printf("%s not matching begin: %i %i %i\n",s,lil.begin()->layerIdx,i,layerIdx);
    for(auto it=lil.begin();it!=lil.end();it++){
        if(it->layerIdx!=i){
            printf("%s not matching: %i %i %i\n",s,*it,i,layerIdx);
            return 0;
        } else {
            // printf("matching: %i %i\n",*it,i);
        }
        i++;
    }
    return 1;
}

void composite_t::arrangeLst(){
    int expectedValue = 0;
    for (auto it = lil.begin(); it != lil.end(); ++it) {
        if (it->layerIdx != expectedValue) {
            it->layerIdx = expectedValue;
        }
        expectedValue++;
    }
}

lil_t* composite_t::insert(int start, int end, int usedIdx, line_c &line, wire_t &wire, int layerIdx){
    auto f = [](const lil_t& a, const lil_t& b) { return a.layerIdx < b.layerIdx; };
    lil_t lilNew = {start,end,usedIdx,&line,&wire,layerIdx,0};
    lilNew.outlier = 0;
    // auto it = std::lower_bound(lil.begin(), lil.end(), lilNew, f);
    auto it = lil.begin();
    for(;it!=lil.end();it++){
        if(it->layerIdx>=layerIdx){
            break;
        }
    }
    lil_t *ld = &(*lil.insert(it, lilNew));
    arrangeLst();
    validateLst("insert",layerIdx);
    return ld;
}

void composite_t::erase(int layerIdx){
    auto f = [&layerIdx](const lil_t& elem) { return elem.layerIdx == layerIdx; };
    auto it = std::find_if(lil.begin(), lil.end(), f);
    if(it != lil.end()){
        lil.erase(it);
    }
    arrangeLst();
    validateLst("erase",layerIdx);
}

void composite_t::preupdate(){
    raw64.convertTo(src64,CV_64F,1,0);
}
#include "core.h"
#include "util.h"
extern core_t srcc;
void composite_t::randn(double mean, double stddev){
    cv::randn(rng,mean,stddev);
    // cv::Mat q = 1.0-srcc.dst.weight.clone();
    // cv::multiply(rng,q,rng);
    cv::add(src64,rng,src64,cv::noArray(),CV_64F);
}

void composite_t::randnw(double mean, double stddev){
    auto w = srcc.dst.weight.clone();
    for(int i=0;i<w.cols*w.rows;i++){
        double r = stddev*pow(1.0-w.at<double>(i),2);
        src64.at<double>(i) += random(-r,r);
    }
}

void composite_t::scale(double lo, double hi){
    double min,max;
    cv::minMaxLoc(src64,&min,&max);
    src64 = (src64-min)*(hi-lo)/(max-min)+lo;
    // cv::randn(rng,mean,stddev);
    // cv::add(raw64,rng,src64,cv::noArray(),CV_64F);
}
#include "util.h"
void composite_t::smooth(double smoothness){
    // cv::blur(src64,src64,cv::Size(smoothness*2+1,smoothness*2+1));
    double r = randomf(smoothness-1,smoothness+1)/10;
    r = smoothness<=1 ? smoothness/10.0 : r;
    r = smoothness;
    cv::GaussianBlur(src64,src64,cv::Size(0,0),r);
}

void composite_t::update(){
    double *sp = (double*)src64.data;
    for(int i=0;i<w*h;i++){
        layerq.at(i).cs = sp[i];
    }
}

computePixel_t& composite_t::at(int px){
    return layerq.at(px);
}

computePixel_t& composite_t::at(int x, int y){
    return layerq.at(y*w+x);
}