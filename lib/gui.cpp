#include "gui.h"
#include <opencv2/opencv.hpp>
#include "opencvex.h"
#include <thread>
#include "wire.h"
#include "core.h"
#include "weight.h"
#include "util.h"
#include "score.h"
using namespace cv;
#include "Instrumentor.h"
#include "lineAA.h"
#include "histogram.h"
#include "metricXCV.h"

extern int lines;
extern int bval_;
extern int methodd;
extern int fast;
extern int w_val;
extern int point_gap;
extern int saver;
extern int noise_sigma;
extern int useds;
extern int threads;
extern int t;
int learnMult_ = 1;
int learnRate_ = 1;
int decayRate_ = 0;
extern float learnRate;
extern float decayRate;
extern int wn;
std::thread th;
int guiRun = 0;
extern int rands;
extern core_t srcc;
extern int pixelScore;
extern int normScore;
extern int fastScore;
int wn2 = 0;
extern int RUN;
int vDir = 0;
int vSrcRng = 0;
int vSmooth = 0;
int v0 = 0;
int v1 = 60;
int v2 = 15;
int vcolorspace = 0;
int vtanh = 255;
int vclc = 100;
int vclc2 = 100;
int vclc3 = 100;
int vpowexpL = 1000;
int vpowexpH = 1000;
int vpowexpS = 1000;
int vdropout = 0;
int vnorm = 1;

TSQueue<guiPush*> guiQ;
void imshow_(const String &winname,const Mat &mat,int mode){
    auto g = new guiPush(winname,mat,mode);
    guiQ.push(g);
}

void imshowMinMax(const String &winname,const cv::Mat &m){
    cv::Mat M = m.clone();
    double l,h;
    cv::minMaxIdx(M,&l,&h);
    char text[100]; sprintf(text,"l: %9.6f h: %9.6f",l,h);
    imshow(winname,M);
    cv::displayStatusBar(winname,text,0);
}

void imshowProc(){
    while(!guiQ.empty()){
        auto g = guiQ.pop();
        switch(g->mode){
            case 0:  imshow(g->winname, g->mat); break;
            case 1:  imshowMinMax(g->winname, g->mat); break;
        }
        delete g;
    }
}

extern int wireIdxSlider;
volatile int keywait = 0;
extern int lines2;
extern float powexp;
extern float powexpL;
extern float powexpH;
float powexpS;
int iterr = 0;
int trans = 50;
int vopa = 0;
std::mutex dstMutex;
int vimprove = 100;
int vselector = 10;
int vlayers = 15;
int vforceScoreMethod = 1;
int vswap = 0;
int vmove = 0;
int vreline = 0;
int vrMax = 1;
int vPercentil = 25;
int vr1 = 10;
int vr2 = 200;
int vnearRange = 1;
int vsamplingRate = 100;
int vsrclo = 100;
int vsrchi = 245;
int vcolor0 = 0;
int vcolor1 = 0;
int vcolor2 = 0;
int vcolor3 = 0;
int dxpos = 0;
int dsmooth = 0;
int dLPF = 0;
int dMF = 0;
int dLR = 0;
int dDR = 0;
int dmin = 0;
double gmin,gmax;
#include "benchmark.h"
double qmean = 0;
double qmean2 = 0;
double qcnt2 = 0;
int B1,B2,B3,B4;
std::mutex Bmutex;
std::deque<double> hscore;
double log0(double x){
    return x;
    if(x>0){
        return log(x);
    } else if(x==0){
        return 0;
    } else {
        return -log(fabs(x));
    }
}
double srcMin,srcMax;
void guiHandler(void *p){
    
    printf("guiHandler\n");
    namedWindow("GUI", WINDOW_AUTOSIZE );
    namedWindow("dst", WINDOW_AUTOSIZE );
    namedWindow("ss", WINDOW_AUTOSIZE );
    // imshow("dst",srcc.dst.img);

    cv::Mat weight(srcc.src.size(),CV_64F);
    weight = 0.5;
    cv::Mat pdweight = weight.clone();
    createTrackbar("xpos","ss",&dxpos,5000);
    createTrackbar("dsmooth","dst",&dsmooth,500);
    createTrackbar("dLPF","dst",&dLPF,5000);
    createTrackbar("dMF","dst",&dMF,1000);
    createTrackbar("dLR","dst",&dLR,1000);
    createTrackbar("dDR","dst",&dDR,1000);
    createTrackbar("dmin","dst",&dmin,1000);

    createTrackbar("opa","dst",&vopa,100);
    createTrackbar("lo","dst",&vsrclo,255);
    createTrackbar("hi","dst",&vsrchi,255);
    createTrackbar("c0","dst",&vcolor0,255);
    createTrackbar("c1","dst",&vcolor1,255);
    createTrackbar("c2","dst",&vcolor2,255);
    createTrackbar("c3","dst",&vcolor3,255);
    double srcMin,srcMax;
    minMaxLoc(srcc.src, &srcMin, &srcMax);
    setTrackbarPos("lo","dst",srcMin);
    setTrackbarPos("hi","dst",srcMax);
    vr1 = srcMin;
    vr2 = srcMax;
    auto colors = srcc.getColors();
    setTrackbarPos("c0","dst",colors.at(0));
    setTrackbarPos("c1","dst",colors.at(1));
    setTrackbarPos("c2","dst",colors.at(2));
    setTrackbarPos("c3","dst",colors.at(3));
    setTrackbarPos("lo","dst",colors.at(0));
    setTrackbarPos("hi","dst",colors.at(3));
    
    // namedWindow("dst", WINDOW_NORMAL );
    createTrackbar("lines","GUI",&lines,20000);
    createTrackbar("lines2","GUI",&lines2,4000);
    // createTrackbar("bval","GUI",&bval_,255);
    // createTrackbar("pixelScore","GUI",&pixelScore,PIXEL_COUNT-1);
    // createTrackbar("normScore","GUI",&normScore,1);
    // createTrackbar("fastScore","GUI",&fastScore,1);
    // createTrackbar("e","GUI",&fast,100);
    // createTrackbar("w","GUI",&w_val,points);
    // createTrackbar("q","GUI",&point_gap,points);
    // createTrackbar("u","GUI",&saver,1000);
    // createTrackbar("noise2","GUI",&noise_sigma,100);
    // createTrackbar("useds","GUI",&useds,10000);
    createTrackbar("threads","GUI",&threads,std::thread::hardware_concurrency());
    // createTrackbar("learn mult","GUI",&learnMult_,100);
    // createTrackbar("learn rate","GUI",&learnRate_,10000);
    // createTrackbar("decay rate","GUI",&decayRate_,10000);
    createTrackbar("rands","GUI",&rands,100000);
    // createTrackbar("wireIdx","GUI",&wireIdxSlider,srcc.dst.wire.size()-1);
    createTrackbar("vDir","GUI",&vDir,3);
    createTrackbar("vSrcRng","GUI",&vSrcRng,125);
    createTrackbar("vSmooth","GUI",&vSmooth,64);
    createTrackbar("v0","GUI",&v0,50);
    createTrackbar("v1","GUI",&v1,3000);
    createTrackbar("v2","GUI",&v2,100);
    createTrackbar("samplingRate","GUI",&vsamplingRate,100);
    createTrackbar("reline","GUI",&vreline,50);
    createTrackbar("colorspace","GUI",&vcolorspace,1);
    createTrackbar("vtanh","GUI",&vtanh,255);
    createTrackbar("vclc","GUI",&vclc,100);
    createTrackbar("vclc2","GUI",&vclc2,100);
    // createTrackbar("vclc3","GUI",&vclc3,100);
    createTrackbar("powexpL","GUI",&vpowexpL,10000);
    createTrackbar("powexpH","GUI",&vpowexpH,10000);
    createTrackbar("powexpS","GUI",&vpowexpS,10000);
    createTrackbar("dropout","GUI",&vdropout,1000);
    // createTrackbar("norm","GUI",&vnorm,255);
    createTrackbar("improve","GUI",&vimprove,100);
    createTrackbar("selector","GUI",&vselector,100);
    createTrackbar("nearRange","GUI",&vnearRange,100);
    createTrackbar("layers","GUI",&vlayers,100);
    createTrackbar("fsm","GUI",&vforceScoreMethod,5);
    createTrackbar("swap","GUI",&vswap,1000);
    createTrackbar("move","GUI",&vmove,32);
    createTrackbar("repeatmax","GUI",&vrMax,1000);
    createTrackbar("percentil","GUI",&vPercentil,100);
    createTrackbar("vr1","GUI",&vr1,255);
    createTrackbar("vr2","GUI",&vr2,255);
    // cv::setTrackbarMin("powexp","GUI",1000);
    guiRun = 1;
    cv::Mat dweightLPF(srcc.src.size(),CV_64F);
    std::vector<cv::Mat> weightav(10);
    for(auto &q : weightav){
        q.create(srcc.src.size(),CV_64F);
        q = 1.0;
    }
    dweightLPF = 0;
    while(guiRun){
        PROFILE_SCOPE("GUI");
        // setTrackbarMax("linesP","dst_",lines-1);
        learnRate = mapf(learnRate_,0,10000,0,learnMult_*1.0);
        decayRate = mapf(decayRate_,0,10000,0,1.0);
        // powexp = mapf(vpowexp,0,5000,0,5.0);
        powexpL = mapf(vpowexpL,0,5000,0,5.0);
        powexpH = mapf(vpowexpH,0,5000,0,5.0);
        powexpS = mapf(vpowexpS,0,5000,0,5.0);
        
        int key = cv::pollKey();
        if(key==27) { RUN = 0; }

        hists_t hists;

        dstMutex.lock();
        extern Paerson__ ps__;
        auto ps___ = ps__;
        cv::Mat raw,src,dst;
        srcc.raw.convertTo(raw,CV_32F,1,0);
        srcc.dst.comp.src64.convertTo(src,CV_32F,1,0);
        srcc.dst_.convertTo(dst,CV_32F,1,0);

        {
            cv::Mat m(500,500,CV_32F);
            cv::Mat n(500,500,CV_32F);
            m = 0;
            n = 0;
            double cnt = raw.cols*raw.rows;
            for(int i=0;i<raw.cols*raw.rows;i++){
                if(pixel8upx(srcc.mask,i)>0){
                    double delta = pixel32fpx(src,i) - pixel32fpx(dst,i);
                    int x = mapf(pixel32fpx(src,i),0,255,0,499);
                    int y = mapf(pixel32fpx(dst,i),0,255,0,499);
                    // pixel32f(m,x,y) = pixel32f(m,x,y)+1;
                    pixel32f(m,x,y) = 1;
                    pixel32f(n,mapf(i,0,cnt,0,499),constrain(mapf(delta,-200,200,500,0),0,499)) += 1;
                }
            }
            normalize(m,m,0,1,NORM_MINMAX);
            normalize(n,n,0,1,NORM_MINMAX);
            for(int i=0;i<499;i++){
                int x = i;
                double y = mapf(i,0,499,0,255);
                y = mapf(y,srcMin,srcMax,vr1,vr2);
                y = mapf(y,0,255,0,499);
                if(!pixelInRange(m,x,y)) continue;
                pixel32f(m,x,y) = 1;
            }
            imshow("reg",m);
            imshow("reg2",n);
        }

        
        hists.raw = raw;
        hists.src = src;
        hists.dst = dst;
        dstMutex.unlock();
        // dLPF = randomf(100,600);
        double fLPF = mapf(dLPF,0,1000,0,1);
        double fMF = mapf(dMF,0,1000,0,1);
        double fLR = mapf(dLR,0,1000,0,1);
        double fDR = mapf(dDR,0,1000,1,0);
        cv::Mat gradient;
        // cv::absdiff(raw,dst,gradient);
        cv::absdiff(src,dst,gradient);
        imshow("diff",gradient/255);
        cv::Mat scoremap;
        double sqq = getMSSIM(src,dst,srcc.mask,scoremap)[0];
        printf("%f\n",sqq);
        // gradient = scoremap.clone();

        gradient.convertTo(gradient,CV_64F,1.0);
        // normalize(gradient,gradient,0,1,NORM_MINMAX);
        // static int qq = 0;
        // qq = (qq+1)%weightav.size();
        // weightav.at(qq) = gradient.clone();
        // gradient = 0;
        // for(auto &q : weightav){
        //     gradient += q;
        // }
        // gradient /= weightav.size();
        // gradient = gradient*(fLPF) + gradient*(1.0-fLPF);
        
        for(int i=0;i<srcc.w*srcc.h;i++){
            if(srcc.mask.at<uint8_t>(i)==0){
                gradient.at<double>(i) = 1;
            }
            gradient.at<double>(i) = mapf(gradient.at<double>(i),0,vtanh,0,3);
            gradient.at<double>(i) = pow(gradient.at<double>(i),mapf(dDR,0,1000,1,4));
            gradient.at<double>(i) = tanh(gradient.at<double>(i));
        }
        extern double dmean;
        extern double ddev;
        printf(">>%f %f\n",dmean,ddev);
        if(dsmooth)
        cv::GaussianBlur(gradient,gradient,cv::Size(0,0),dsmooth/10.0);
        for(int i=0;i<srcc.w*srcc.h;i++){
            double val = gradient.at<double>(i);
            double p = mapf(dLPF,0,1000,0.1,10);
            gradient.at<double>(i) = pow(val,p);
        }
        // double rlo = DBL_MAX;
        // double rhi = DBL_MIN;
        // for(int i=0;i<srcc.w*srcc.h;i++){
        //     if(srcc.mask.at<uint8_t>(i)==0){
        //         gradient.at<double>(i) = 0;
        //     }
        //     rlo = std::min(rlo,weight.at<double>(i));
        //     rhi = std::min(rhi,weight.at<double>(i));
        // }
        cv::Mat rng;
        rng = gradient.clone();
        rng = 0;
        cv::randn(rng,0.5,0.5);
        cv::minMaxLoc(gradient,&gmin,&gmax,nullptr,nullptr,srcc.mask);
        normalize(gradient,gradient,dmin/1000.0,1,NORM_MINMAX);
        // gradient = (gradient+rng)/2.0;
        weight = weight*(1.0-fLR) + gradient*(fLR);
        // weight = weight*(1.0-fDR);
        // weight = weight + dweight*fLR;
        // cv::Mat dweight = gradient*fLR + pdweight*fMF;
        // weight = weight*fDR + dweight;
        // pdweight = dweight;
        // cv::multiply(weight,gradient,weight);
        // weight = gradient.clone();
        // normalize(weight,weight,0,1,NORM_MINMAX);
        // for(int i=0;i<srcc.w*srcc.h;i++){
        //     double val = weight.at<double>(i);
        //     double p = mapf(dLPF,0,1000,0.1,10);
        //     weight.at<double>(i) = pow(val,p);
        // }
        double gap = 4;
        {
            double l = DBL_MAX;
            double h = DBL_MIN;
            double l2 = DBL_MAX;
            double h2 = DBL_MIN;
            std::vector<double> lst;
            std::vector<double> lst2;
            dstMutex.lock();
            // qmean2 = 0;
            for(auto &q : srcc.dst.comp.lil){
                double val = q.sr.score;
                double val2 = q.sr.score2;
                // if(val2!=0) printf("%f\n",val2);
                qmean += val;
                lst.push_back(val);
                lst2.push_back(val2);

                l = min(l,val);
                h = max(h,val);

                l2 = min(l2,val2);
                h2 = max(h2,val2);

                // qmean2 += val2;
                qcnt2 += 1;
            }
            // h2 = 1;
            // qmean2 = qmean2/qcnt2;
            // qmean /= srcc.dst.comp.lil.size();
            dstMutex.unlock();
            double lh = max(abs(l),abs(h));
            // printf("%f %f  %f %f\n",l,h,l2,h2);
            cv::Mat m(800,800,CV_8U);
            m = 0;
            // h = (floor(h*1000)+1)/1000.0;
            for(int i=0;i<lst.size();i++){
                int x = round(mapf(i,0,lst.size(),gap,m.cols-1-gap));
                // int x = round(i-dxpos);
                // double ymult = mapf(dxpos,0,100,1,10);
                int y =  round((mapf(lst.at(i), l, h, m.rows-1-gap,gap)));
                int y2 = round((mapf(log0(lst2.at(i)),log0(l2),log0(h2),m.rows-1-gap,gap)));
                // int y0 = round(mapf(0,-lh,lh,m.rows-1,0));
                // int ym = round(mapf(qmean,-lh,lh,m.rows-1,0));
                if(pixelInRange(m,x,y)){
                    pixel8u(m,x,y) = 32;
                }
                if(pixelInRange(m,x,y2)){
                    pixel8u(m,x,y2) = 255;
                }
            }
            imshow("ss",m);
            char filename[1000]; sprintf(filename,"%f\n%f\n%f\n%f\n%f\n\n",l,h,l2,h2,qmean2);
            cv::displayOverlay("ss",filename,0);
        }
        if(0)
        {
            double min,max;
            minMaxIdx(weight,&min,&max,nullptr,nullptr,srcc.mask);
            for(int i=0;i<srcc.w*srcc.h;i++){
                double val = mapf(weight.at<double>(i),0,max,0,1);
                if(min==max){
                    val = 1;
                }
                weight.at<double>(i) = val;
            }
        }
        dstMutex.lock();
        for(int i=0;i<srcc.w*srcc.h;i++){
            double val = mapf(weight.at<double>(i),0,1,0,1);
            // double val = mapf(scoremap.at<float>(i),0,1,0,1);
            srcc.dst.weight.at<double>(i) = val;
        }
        dstMutex.unlock();

        dstMutex.lock();
        qmean2 = 0;
        double qmean2cnt = 0;
        std::vector<int> qmeanarr;
        qmeanarr.resize(1000);
        if(srcc.dst.comp.lil.size()>0)
        {
            double l1 = DBL_MAX;
            double h1 = DBL_MIN;
            double l2 = DBL_MAX;
            double h2 = DBL_MIN;
            double l3 = DBL_MAX;
            double h3 = DBL_MIN;
            
            for(auto &q : srcc.dst.comp.lil){
                if(    q.sr.score==DBL_MAX 
                    || q.sr.score2==DBL_MAX 
                    || q.sr.score3==DBL_MAX) continue;
                l1 = std::min(l1,q.sr.score);
                h1 = std::max(h1,q.sr.score);
                l2 = std::min(l2,q.sr.score2);
                h2 = std::max(h2,q.sr.score2);
                l3 = std::min(l3,q.sr.score3);
                h3 = std::max(h3,q.sr.score3);
                q.outlier = 0;
            }
            // h2 = 1;
            for(auto &q : srcc.dst.comp.lil){
                if(    q.sr.score==DBL_MAX 
                    || q.sr.score2==DBL_MAX 
                    || q.sr.score3==DBL_MAX
                    || q.sr.score2==DBL_MIN
                    || isnan(q.sr.score2)
                    || q.sr.score2==0) continue;
                    double v2 = q.sr.score2;
                    // v2 = mapf(v2,l2,h2,0,qmeanarr.size());
                    // qmeanarr[round(v2)]++;
                    if(q.sr.score2!=0){

                        qmean2 += v2;
                        qmean2cnt += 1;
                    }
                if(q.sr.score2>(h2*0.9)){
                    q.outlier = 1;
                }
            }
            qmean2 /= qmean2cnt;
            // int qmeanmaxidx = 0;
            // for(auto v : qmeanarr){
            //     if(v>qmeanmax){
            //         qmeanmax = v;
            //     }
            // }
            // qmean2 = qmeanmax;
            // qmean2 = mapf(qmean2);
            double slope = (h2-l2)/(h1-l1);
            // printf("%f\n%f\n%f\n%f\n%f\n\n",l1,h1,l2,h2,qmean2);
            cv::Mat m(800,800,CV_8U);
            m = 0;
            {
                int x0 = round(mapf(0,l1,h1,gap,m.cols-1-gap));
                int y0 = round(mapf(0,l2,h2,m.rows-1-gap,gap));
                for(int x=0;x<m.cols-1;x++){
                    if(pixelInRange(m,x,y0)){
                        pixel8u(m,x,y0) = 100;
                    }
                }
                for(int y=0;y<m.rows-1;y++){
                    if(pixelInRange(m,x0,y)){
                        pixel8u(m,x0,y) = 100;
                    }
                }
                int hsize = hscore.size();
                double hsl = DBL_MAX;
                double hsh = DBL_MIN;
                for(auto v : hscore){
                    hsl = std::min(hsl,v);
                    hsh = std::max(hsh,v);
                }
                for(int i=0;i<hsize;i++){
                    int x = round(mapf(i,0,hsize-1,gap,m.cols-1-gap));
                    int y = round(mapf(hscore.at(i),hsl,hsh,m.rows-1-gap,gap));
                    if(pixelInRange(m,x,y)){
                        pixel8u(m,x,y) = 180;
                    }
                }
                // if(pixelInRange(m,x,y)){
                //     pixel8u(m,x,y) = 255;
                //     pixel8u(m,x+1,y) = 255;
                //     pixel8u(m,x-1,y) = 255;
                //     pixel8u(m,x,y+1) = 255;
                //     pixel8u(m,x,y-1) = 255;
                // }
            }
            for(auto &q : srcc.dst.comp.lil){
                int x = round(mapf(q.sr.score, l1,h1,gap,m.cols-1-gap));
                int y = round(mapf(q.sr.score2,l2,h2,m.rows-1-gap,gap));
                // int z = round(mapf(q.sr.score3,l3,h3,16,255));
                if(pixelInRange(m,x,y)){
                    pixel8u(m,x,y) = 255;
                }
            }
            
            imshow("objectives",m);
            // objectives.resize(0);
        }

        if(0)
        {
            cv::Mat qmap = gradient.clone();
            gradient.convertTo(qmap,CV_64F);
            double l = DBL_MAX;
            double h = DBL_MIN;
            int binmax = 0;
            for(int i=0;i<qmap.cols*qmap.rows;i++){
                if(srcc.mask.at<uint8_t>(i)==0) continue;
                double v = qmap.at<double>(i);
                l = min(l,v);
                h = max(h,v);
            }
            std::vector<int> bins;
            bins.resize(qmap.cols);
            for(int i=0;i<qmap.cols*qmap.rows;i++){
                if(srcc.mask.at<uint8_t>(i)==0) continue;
                double v = qmap.at<double>(i);
                int index = round(mapf(v,l,h,0,qmap.cols-1));
                bins.at(index) += 1;
                binmax = max(binmax,bins.at(index));
            }
            // printf("%i\n",binmax);
            qmap = 0;
            cv::Mat qq = qmap;
            for(int i=0;i<bins.size();i++){
                int y = round(mapf((bins.at(i)),(0),(binmax),qq.rows-1,0));
                int x = round(mapf(i,0,bins.size(),0,qq.cols));
                // printf("%i %i %i\n",x,y,bins.at(i));
                if(pixelInRange(qq,x,y)){
                    pixel64f(qq,x,y) = 1;
                }
            }
            imshow("qq",qq);
        }
        dstMutex.unlock();
        Mat m = weight.clone();
        double min,max;
        minMaxIdx(m,&min,&max,nullptr,nullptr,srcc.mask);
        normalize(m,m,0,1,NORM_MINMAX);
        imshowAs8U("gradient",gradient*255);
        imshowAs8U("weight",srcc.dst.weight*255);
        // imshowAs8U("weight2",m*255);

        double score = getImgErrorProcentage(src,dst,srcc.mask,powexpS);
        double score2 = getImgPaerson(dst,src,srcc.mask,powexpS);
        cv::Mat mix = (src*vopa + dst*(100.0-vopa))/100.0;
        mix.convertTo(mix,CV_8U,1,0);
        int skip = 0;
        hists.mix = mix;
        // cv::resize(mix,mix,cv::Size(500,500));
        imshow("dst", mix);
        Bmutex.lock();
        char filename[100]; sprintf(filename,"iter: %i line: %4i score: %12.9f\n%i %i\n%f"
            ,iterr,srcc.lineIdx,score,B1,B2,score2);
        Bmutex.unlock();
        cv::displayOverlay("dst",filename,0);
        // printf("%s\n",filename);
        imshowProc();
        // float qq = genTriangle(iterr,10000,50,80,4);

        
        
        imshow("histogram",getHistogram(hists));
        waitKey(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    destroyAllWindows();
    // printf("thread break   GUI\n");
    // raise(SIGINT); // powoduje deadlock bo wątek staje się unjoinable
}

void guiDestroy(){
    guiRun = 0;
    // printf("thread joining GUI %i\n",th.joinable());
    if(th.joinable()) th.join();
    // printf("thread joined  GUI\n");
    destroyAllWindows();
}

void guiCreate(){
    printf("guiCreate\n");
    th = std::thread(guiHandler, nullptr);
    while(guiRun==0);
}