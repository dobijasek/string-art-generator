#include "drawLines.h"
#include <string>
#include "progress_bar.hpp"
#include "find.h"
#include <opencv2/opencv.hpp>
#include "opencvex.h"
#include "util.h"
#include "score.h"
#include "gui.h"
#include "core.h"
#include "history.h"
#include "weight.h"
#include <opencv2/core/utils/logger.hpp>
#include "Instrumentor.h"
#include "drawLines2.h"
#include "graph.h"
#include <algorithm>
#include "testQueue.h"

using namespace cv; 

const int wire_cnt = 4;
const int wire_mul = 200;
extern int saver;
int bval_ = 253;
int methodd = 0;
int iters = 0;
int useds = 10000;
int threads = 0;
int densee = 6;
int t = 1;
float learnRate = 0;
float decayRate = 0;
int wn = 0;
int useBoost = 1;



core_t srcc;

extern int lines;
extern int lines2;
extern int noise_mean;
extern int noise_sigma;
extern int darkness1;
extern int darkness2;
extern int darkness3;
extern int fast;
extern int point_gap;
extern int w_val;
extern int g_val;
extern int sides;
int pixelScore=0;
int normScore=1;
int fastScore=0;
float score_ = 0;
int iter_ = 0;
int rands = 30;
int wireIdxSlider = 0;
extern std::string input1_image; 
extern std::string input2_image;
extern volatile int keywait;
extern volatile int randsTotal;
extern int vswap;
void BEGIN(){
  srcc.reset(bval_);
  srcc.scoreMode = {pixelScore,normScore,fastScore};
}

void PRE(){
}

void POST(){
}

void END(){
      static int iter = 0;
      iter++;
      iter_ = iter;
      if(iter%100!=0) return;
    //   float score = srcc.getImgScore();
    //   score_ = score;
      // imwrite("project/srcMap2.png",srcMap2);
      // imwrite("project/srcMap.png",srcMap);
      // applyPenalty(&dst,wires,nails,best->start,best->end);
    //   if(iter%100==0) printf("iter %i: %.8f\n",iter,score);
      // srcc.dstDone = srcc.dst.img.clone();
      // cv::Mat layer = srcc.dst.layer.clone();
      // normalize(layer,layer,0,255,NORM_MINMAX,CV_8U);
      // imshow_("layer_",layer);
      // imshow_("dst_p",dst222);
      Mat denseNormalized = srcc.dst.dense.clone();
      normalize(denseNormalized,denseNormalized,0,256,NORM_MINMAX);
      imshow_("dense",denseNormalized,1);

      imshow_("used",srcc.used);
}
#include "pair.h"
#include <thread>
#include "pool.h"
#include <iterator>
#ifdef USE_CUDA
#include <cuda_runtime_api.h>
#include "ckernel.h"
#endif
extern int v1;
extern int iterr;
extern int vDir;
extern int vSrcRng;
extern int vSmooth;
extern int vtanh;
float powexp = 1;
float powexpL = 1;
float powexpH = 1;
extern std::mutex dstMutex;
extern int vimprove;
extern int scoreMethod;
extern int vselector;
extern int vlayers;
extern int vreline;
extern int vsrclo;
extern int vsrchi;
extern int vcolor0;
extern int vcolor1;
extern int vcolor2;
extern int vcolor3;
extern int v0;
extern int vdropout;
double powexp1 = 1;
double powexp2 = 1;
double powexp3 = 1;
extern scoreParam_t sm1;
extern scoreParam_t sm2;
extern scoreParam_t sm3;
extern int vclc;
extern std::deque<double> hscore;
// #define DRAW_HIST
#define DRAW_RAND
Paerson__ ps__;
Paerson__ ps__drop;
Paerson__ ps__G;
double intesityRng = 1;
extern int NAILSCNT;
bool isvalidrange(int start, int end){
    while(start<0) start += NAILSCNT;
    while(end<0) end += NAILSCNT;
    start = start%NAILSCNT;
    end = end%NAILSCNT;
    if(start>end){
        end += NAILSCNT;
    }
    return (end-start)<10 ? 0 : 1;
}
float drawLines(){
    std::vector<int> abc;
    //     abc.resize(1000);
    //     abc.at(100000) = 66;
  
  cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_ERROR);
  printf("opencv log level: %i\n",cv::utils::logging::getLogLevel());
  printf("init canvas\n");
  srcc.init(input1_image,NAILSCNT,NAILSCNT,4);
  if(!input2_image.empty()){
    srcc.src = imread(input2_image,IMREAD_GRAYSCALE);
    srcc.raw = srcc.src.clone();
  }
  srcc.line_init(); // TODO: przenieść do init
  
  // ProgressBar *generatingImage = new ProgressBar(lines, "Generating image");
  extern int RUN;
  
  weightLoad();
  guiCreate();
  int qqq = 1000000;
  while(qqq--);
  printf("START\n");
  
  while(1){
    BEGIN();
    // if(0)
    #ifdef DRAW_HIST
    {
        for( int lineIdx=0; lineIdx<lines; lineIdx++ ){
            if(!RUN) break;
            PRE();
            best_t best = find(srcc);
            if(!best.found()) break;
            srcc.drawLine(best);
            dstMutex.lock();
            srcc.dst_ = srcc.dst.img.clone();
            dstMutex.unlock();
            POST();
        }
        END();
        // continue;
        srcc.cws.resize(srcc.src.cols*srcc.src.rows);
    }
    
    std::vector<best_t> hist = srcc.hist;
    BEGIN();
    for(int i=0;i<hist.size();i++){
      srcc.drawLine3(hist.at(i).start,hist.at(i).end,*hist.at(i).wire,hist.at(i).layerIdxDraw);
      hist.at(i).layerIdxDraw = i;
    }
    #endif

    #ifdef DRAW_RAND
    BEGIN();
    std::vector<best_t> hist;
    for(int i=0;i<lines;i++){
        int start;
        int end;
        int usedIdx;
        int wireIdx = random(0,srcc.dst.wire.size()-1);
        while(1){
            start = random(0,srcc.dst.nail.size()-1);
            end = random(0,srcc.dst.nail.size()-1);
            if(start==end) continue;
            if(start>end){
                end+=NAILSCNT;
            }
            if(!isvalidrange(start,end)) continue;
            start = start%NAILSCNT;
            end = end%NAILSCNT;
            if(start>=NAILSCNT || start<0 || end>=NAILSCNT || end<0) continue;
            usedIdx = getNailsIndex(start,end,srcc.dst.nail.size());
            if( srcc.used.data[usedIdx]>0 ) continue;
            break;
        }
        srcc.drawLine3(start,end,srcc.dst.wire.at(wireIdx),i);
    }
    #endif
    
    Instrumentor::Get().BeginSession("Session Name");
    int dirF = 1;

    #ifdef USE_CUDA
    uint8_t  *d_src;
    uint8_t  *d_dst;
    uint16_t *d_layer;
    float    *d_score;
    int imgSize = srcc.src.cols*srcc.src.rows;
    int scoreSize = srcc.dst.nail.size()*srcc.dst.nail.size();
    auto e1 = cudaMalloc( (void**)&d_src,   imgSize*sizeof(*d_src) );
    auto e2 = cudaMalloc( (void**)&d_dst,   imgSize*sizeof(*d_dst) );
    auto e3 = cudaMalloc( (void**)&d_layer, imgSize*sizeof(*d_layer) );
    auto e3 = cudaMalloc( (void**)&d_score, scoreSize*sizeof(*d_score) );
    // cudaMemcpy(d_score, ha, scoreSize*sizeof(*d_score), cudaMemcpyHostToDevice);
    if(e1||e2||e3){
      printf("CUDA error\n");
      while(1);
    }
    runCudaKernel();
    #endif
    
    // int layerIdxErase = 0;
    std::vector<double> graph1;
    std::vector<double> graph2;
    while(1)
    {
        PROFILE_SCOPE("redrawer");
        // intesityRng = randomf(powexpL,powexpH);
        powexp = randomf(powexpL,powexpH);
        ps__.p = powexp;
        ps__drop.p = powexp;
        ps__G.p = powexp;
        powexp1 = randomf(0.8,1.5);
        powexp2 = randomf(2,4);
        powexp3 = randomf(5,6);
        sm1.randomize(1,1);
        sm2.randomize(2,4);
        sm3.randomize(5,6);
        sm1.normalize = 0;
        // sm2.normalize = 1;
        // sm3.normalize = 1;
        scoreMethod = random(1,2);
        const auto colors_ = srcc.getColors();
        // trans_ = genTriangle(iterr,10000,0.6,0.85,0.01);
        
        // vtanh = random(1,255);
        // powexp - randomf(3,7);
        //=============================================================
        // randomize src
        //=============================================================
        // if(0)
        {
            const int srcRngInterval = 50;
            if((iterr%srcRngInterval)==0){
                dstMutex.lock();
                srcc.dst.comp.preupdate();
                srcc.dst.wire.at(0).color = vcolor0;
                srcc.dst.wire.at(1).color = vcolor1;
                srcc.dst.wire.at(2).color = vcolor2;
                srcc.dst.wire.at(3).color = vcolor3;
                srcc.dst.comp.scale(vsrclo,vsrchi);
                if(vSmooth>0)
                srcc.dst.comp.smooth(vSmooth);
                srcc.dst.comp.randn(0,vSrcRng);
                srcc.dst.comp.update();
                dstMutex.unlock();
            }
        }
        extern double dmean;
        extern double ddev;
        dmean = 0;
        ddev = 1;
        dstMutex.lock();
        ps__.clear();
        ps__G.clear();
        ps__.init(srcc.dst.img.cols,srcc.dst.img.rows);
        ps__G.init(srcc.dst.img.cols,srcc.dst.img.rows);
        extern double pp;
        pp = random(2,8);
        for(int i=0;i<srcc.src.cols*srcc.src.rows;i++){
            if(pixel8upx(srcc.mask,i)==0) continue;
            // double A = pixel8upx(srcc.src,i);
            double A = pixel64fpx(srcc.dst.comp.src64,i);
            double AG = pixel64fpx(srcc.dst.comp.src64,i);
            double B = pixel32fpx(srcc.dst.img,i);
            double W = pixel64fpx(srcc.dst.weight,i);
            // ps__.add(i,A,B,W);
            ps__.add(i,A,B,W);
            ps__G.add(i,AG,B,W);
            // printf("%f\n%f\n\n\n",A,B);
        }
        dmean = ps__.SDAB/ps__.N;
        ddev = fabs(ps__.qq());
        ps__.clear();
        ps__G.clear();
        ps__.init(srcc.dst.img.cols,srcc.dst.img.rows);
        ps__G.init(srcc.dst.img.cols,srcc.dst.img.rows);
        for(int i=0;i<srcc.src.cols*srcc.src.rows;i++){
            if(pixel8upx(srcc.mask,i)==0) continue;
            // double A = pixel8upx(srcc.src,i);
            double A = pixel64fpx(srcc.dst.comp.src64,i);
            double AG = pixel64fpx(srcc.dst.comp.src64,i);
            double B = pixel32fpx(srcc.dst.img,i);
            double W = pixel64fpx(srcc.dst.weight,i);
            // ps__.add(i,A,B,W);
            ps__.add(i,A,B,W);
            ps__G.add(i,AG,B,W);
            // printf("%f\n%f\n\n\n",A,B);
        }
        hscore.push_back(ps__G.compute3());
        if (hscore.size() > 5000)
            hscore.pop_front();
        dstMutex.unlock();
        //=============================================================
        // dropout
        //=============================================================
        {
            dstMutex.lock();
            PROFILE_SCOPE("dropout A");
            if(vdropout>0)
            {
                std::mutex ps__mutex;
                ps__drop.clear();
                ps__drop.init(srcc.dst.img.cols,srcc.dst.img.rows);
                for(auto &q : srcc.dst.comp.lil){
                    // if(q.layerIdx<5000) continue;
                    int d = random(1,1000);
                    if(d<vdropout){
                        TASKER.push({[&](){
                            int lie = q.layerIdx;
                            auto sr = srcc.getLineScoreFromLayer(lie);
                            ps__mutex.lock();
                            ps__drop.sub(sr.pearson);
                            ps__mutex.unlock();
                            q.drop = 1;
                        }});
                    }
                }
            }
            dstMutex.unlock();
        }

        // printf("%i\n",iterr);
        // #define HIST(r) (*std::next(srcc.dst.comp.lil.begin(),r))
        dstMutex.lock();
        //=============================================================
        // select worse line
        //=============================================================
        std::vector<scoreResult_t> sr_worst_lst;
        // scoreResult_t sr_worst;
        int qsel = random(1,100)<=vclc;
        extern int vclc2;
        int qsel2 = random(1,100)<=vclc2;
        // qsel = 0;
        std::mutex sr_worst_mutex;
        extern double qmean2;
        double qmean2_ = qmean2<0 ? 0 : qmean2;
        {
            PROFILE_SCOPE("select worse layerIdx");
            auto cmpWorst = [qsel,qsel2,qmean2_](const scoreResult_t &a,const scoreResult_t &b){
                return (a.score<b.score);
                // return (a.pearson.compute3()<b.pearson.compute3());
                // return (a.score2<b.score2);
                // return (a.score3<b.score3);
                // return qsel2 ? (a.score3<b.score3) : (a.score2<b.score2);
                // return qsel ? (a.score3<b.score3) : (a.score<b.score);
                // return qsel ? (fabs(qmean2_-a.score2)>fabs(qmean2_-b.score2)) : (a.score<b.score);
                // return fabs(qmean2_-a.score2)>fabs(qmean2_-b.score2);
            };
            // dstMutex.lock();
            v1 = v1<1 ? 1 : v1;
            int layerIdxEraseCount = v1<=30 ? v1 : random(30,v1);
            for(int i=0;i<layerIdxEraseCount;i++){
                int layerIdxErase = random(0,srcc.layers()-1);
                TASKER.push({[&,layerIdxErase](){
                    auto sr = srcc.getLineScoreFromLayer(layerIdxErase);
                    sr_worst_mutex.lock();
                    if(sr.len>0){
                        sr_worst_lst.push_back(sr);
                    }
                    sr_worst_mutex.unlock();
                }});
            }
            TASKER.wait();
            // dstMutex.unlock();
            if(sr_worst_lst.size()==0) continue;
            sort(sr_worst_lst.begin(),sr_worst_lst.end(),cmpWorst);
            extern int v2;
            int rg = v2;
            if(sr_worst_lst.size()>rg) sr_worst_lst.resize(rg);
            // sr_worst = sr_worst_lst.at(0);
        }
        

        //=============================================================
        // testing lines
        //=============================================================
        double qmean = 0;
        for(auto &q : srcc.dst.comp.lil){
            double val = q.sr.score;
            qmean += val;
        }
        qmean = qmean/srcc.dst.comp.lil.size();
        scoreResult_t sr_best;
        scoreResult_t sr_best_sr_erase;
        double score1 = random(1,100)<=vimprove ?  0 : -1;
        // if(qsel) score1 = -1;
        // score1 = 0;
        double score2 = 99999999;
        double score3 = DBL_MAX;
        double scoreEmptys = 9999999999;
        std::mutex sr_best_mutex;
        std::vector<scoreResult_t> srv;
        auto cmpBest = [](const scoreResult_t &a,const scoreResult_t &b){
            bool a_less_b = a.score<b.score && a.score2<=b.score2 && a.score3<=b.score3;
            return a_less_b || a.empty!=b.empty;
        };
        TSQueue<aaa_t> queue;
        int bcnt = 0;
        
        {
            PROFILE_SCOPE("testing lines");
            auto testLinesF = [&](){
                PROFILE_SCOPE("getScore");
                auto q = queue.pop();
                int start = q.start;
                int end = q.end;
                if(!isvalidrange(start,end)) return;
                int usedIdx = q.usedIdx;
                int layerIdxErase__ = q.layerIdxErase;
                int layerIdxDraw__ = q.layerIdxDraw;
                auto &sr_erase = q.sr_erase;
                if(layerIdxErase__!=sr_erase.layerIdxErase){
                    printf("%f\n",layerIdxErase__,sr_erase.layerIdxErase);
                }
                std::vector<scoreResult_t> sr_draw_lst = getLineScoreqq(colors_,start,end,layerIdxDraw__,layerIdxErase__);
                int xxx = 0;
                for(int wireIdx=0;wireIdx<sr_draw_lst.size();wireIdx++){
                    auto &sr_draw = sr_draw_lst[wireIdx];
                    sr_best_mutex.lock();
                    // objectives.push_back({sr_draw.score,sr_draw.score2,0});
                    double ds1 = -sr_erase.score+sr_draw.score;
                    auto ps__p = ps__;
                    
                    // ds1 = sr_draw.score;
                    // if(qsel){
                    //===================================
                    // double ds1p = ps__.qq();
                    // ps__p.sub(sr_erase.pearson);
                    // ps__p.add(sr_draw.pearson);
                    // if(vdropout>0){
                    //     ps__p.sub(ps__drop);
                    // }
                    // ds1 = ps__p.qq()-ds1p;
                    // ds1 = ds1;
                    //===================================
                        // ds1 = fabs(ps__p.qq()-sr_draw.pearson.qq());
                        // ds1 = ps__p.compute5()-ds1p;
                    // } else {
                    //     double ds1p = ps__.compute();
                    //     ps__p.sub(sr_erase.pearson);
                    //     ps__p.add(sr_draw.pearson);
                    //     if(vdropout>0){
                    //         ps__p.sub(ps__drop);
                    //     }
                    //     ds1 = ps__p.compute()-ds1p;
                    // }
                    // ds1 =-sr_draw.pearson.compute5();
                    // sr_draw.pearson.print();
                    // printf("%f\n",ds1);
                    // double ds2 = -fabs(sr_erase.score2-qmean2_)+fabs(sr_draw.score2-qmean2_);
                    double ds2 = -sr_erase.score2+sr_draw.score2;
                    // ds2 = fabs(qmean2_-sr_erase.score2)-fabs(qmean2_-sr_draw.score2);
                    // ds2 = sr_draw.score2;
                    // if(!qsel){
                        // ds1 = ds2;
                    // }
                    // double sdraw = -(-ds1-sr_erase.score);
                    // ds2 = (qmean-(-ds1-sr_erase.score));
                    double ds3 = -sr_erase.score3+sr_draw.score3;
                    // auto qq = random(1,100)<=vimprove ? sr_draw.score<qmean : 1;
                    // if(ds1>score1 && ds2<score2){
                    // if(ds1<score1 && sr_draw.score2<=vclc && sr_draw.score3<=vclc){
                    // if(ds1<score1){
                    // printf("%f\n",ds1);
                    // int war0 = ds1>score1;
                    // int war1 = ds1>score1 && ds2<=score2;
                    // int war2 = ds2>=score2;
                    // war2 = ds2>=(-fabs(qmean2-sr_erase.score2)+fabs(qmean2-sr_draw.score2));
                    // int qwar = qsel ? (wireIdx==0 ? war2 : war1) : war0;
                    // if(sr_draw._emptys<=scoreEmptys)
                    // if(qsel==0 ? (ds1>score1) : ((ds2>score2)&&(sr_draw.score2!=0)))
                    // if((qsel==0 ? (ds1>score1) : (ds2>=score2)&&(ds1>score1)))
                    // if(ds2>=score2 && sr_draw.score2<-10 && sr_draw.score2>-50)
                    // xxx = 0;
                    // if(wireIdx==0){
                    //     xxx = sr_draw.score2<=111 && sr_draw.score2>22;
                    //     xxx = xxx==0 ? (sr_draw.score2<=score2) : (xxx);
                    //     xxx = xxx==1 ? (ds1>score1) : (xxx);
                    // } else {
                    //     xxx = ds1>score1;
                    // }
                    if(sr_draw.len>10)
                    // if(xxx)
                    if(ds1>score1)
                    // if(ds2>score2)
                    {
                        // if(ds1>score1 && ds2>=score2 && sr_draw.score2<=-30){
                        // if(sr_draw.score>ps__.compute() && sr_draw.score>score1){
                        // if((cmpBest(sr_draw,sr_best) && cmpBest(sr_draw,sr_erase))){
                        // printf("%f\n%f\n\n\n",sr_draw.score,ps__.compute());
                        // printf("%f %f    %f %f\n",ds1,score1,ds2,score2);
                        score1 = ds1;
                        score2 = sr_draw.score2;
                        score3 = ds3;
                        scoreEmptys = sr_draw._emptys;
                        // score3 = sr_draw.score3;
                        sr_best = sr_draw;
                        sr_best.wire = &srcc.dst.wire[wireIdx];
                        sr_best.start = start;
                        sr_best.end = end;
                        sr_best_sr_erase = sr_erase;
                        bcnt++;
                    }
                    sr_best_mutex.unlock();
                }
            };
            auto testLines = [&](){
                int q = queue.size();
                for(int i=0;i<q;i++){
                    TASKER.push({testLinesF});
                }
                TASKER.wait();
            };
            extern int vrMax;
            int repeatsMax = vrMax;
            int repeats = 0;
            while(repeats++<repeatsMax)
            {
                if(random(1,100)>vselector || repeats>1){
                    // random
                    queue.clear();
                    tqRandom(queue,sr_worst_lst,-1);
                    testLines();
                    if(sr_best.empty==0)
                    {
                        queue.clear();
                        pairGeneratorNear(srcc,sr_best.start,sr_best.end,2,[&](int start, int end,int usedIdx){
                            int lie = sr_best.layerIdxErase;
                            int lid = sr_best.layerIdxDraw;
                            lid = constrain(lid,0,srcc.layers()-1);
                            lie = constrain(lie,0,srcc.layers()-1);
                            queue.push({start,end,usedIdx,lid,lie,sr_best_sr_erase});
                        });
                        testLines();
                        break;
                    }
                    // printf("random: %i\n",sr_best.empty);

                } else {
                    // near
                    extern int vnearRange;
                    queue.clear();
                    tqNear(queue,sr_worst_lst,srcc.layers(),vnearRange);
                    testLines();

                    queue.clear();
                    tqNear(queue,sr_worst_lst,0,vnearRange);
                    testLines();
                    // printf("near: %i\n",sr_best.empty);
                    break;
                }
                if(sr_best.empty==0){
                    // repeats = 0;
                    break;
                }
            }
            if(repeats>1){
                // printf("repeats %i\n",repeats);
            }
        }
        // objectives.push_back({sr_best.score,sr_best.score2,1});
        dstMutex.unlock();
        //=============================================================
        // redrawing line
        //=============================================================
        {
            PROFILE_SCOPE("redrawing line");
            if( sr_best.empty!=1 ){
                auto best_ = sr_best;
                // if(best_.score3>0) printf("%f\n",best_.score3);
                dstMutex.lock();
                int lie = best_.layerIdxErase;
                int lid = best_.layerIdxDraw;
                auto &be = *std::next(srcc.dst.comp.lil.begin(),lie);
                auto &bd = *std::next(srcc.dst.comp.lil.begin(),lid);
                int liee = be.layerIdx;
                // auto srEraseValidate = srcc.getLineScoreFromLayer(liee);
                // double sa = getImgPaerson(srcc.dst.comp.src64,srcc.dst.img,srcc.mask,1);
                srcc.eraseLine3(liee);
                // double se = getImgPaerson(srcc.dst.comp.src64,srcc.dst.img,srcc.mask,1);
                int lidd = lie==lid? liee : bd.layerIdx;
                srcc.drawLine3(best_.start,best_.end,*best_.wire,lidd);
                // double sb = getImgPaerson(srcc.dst.comp.src64,srcc.dst.img,srcc.mask,1);
                // printf("before:\t%f\nerase:\t%f\nafter:\t%f\nscore1:\t%f\naft-bef:%f\n\n"
                //     ,sa,se,sb
                //     ,score1,sb-sa);
                // printf("%f %f %f    %f %f %f %f\n",se-sa,srEraseValidate.score,sr_best_sr_erase.score
                // ,sb-se,sb-sa,best_.score,score1);
                // printf("%f\n%f\n%f\n\n",sa,sb,score1);
                srcc.getLineScoreFromLayer(lidd);
                dstMutex.unlock();
                // printf("%i %i %i %i %i %i\n",srcc.dst.comp.lil.size(),lie,lid,lidd,best_.start,best_.end);
            }
            // graph2.push_back(best.score);
        }
        iterr++;

        //=============================================================
        // reline
        //=============================================================
        {
            dstMutex.lock();    
            for(int i=0;i<vreline;i++){
                int li = random(0,srcc.layers()-1);
                auto bq = srcc.eraseLine3(li);
                int s = bq.start;
                int e = bq.end;
                srcc.line.at(getNailsIndex(s,e,srcc.dst.nail.size())).init(srcc.src,srcc.dst.img,srcc.mask,srcc.dst.nail,s,e);
                srcc.drawLine3(bq.start,bq.end,*bq.p_wire,bq.layerIdx);
                srcc.getLineScoreFromLayer(bq.layerIdx);
            }
            dstMutex.unlock();
        }
        //=============================================================
        // dropout
        //=============================================================
        {
            PROFILE_SCOPE("dropout B");
            dstMutex.lock();
            for(auto &q : srcc.dst.comp.lil){
                q.drop = 0;
            }
            dstMutex.unlock();
        }

        //=============================================================
        // random swap
        //=============================================================
        {
            PROFILE_SCOPE("swap");
            if(vswap)
            for(int i=0;i<10;i++)
            {
                int sd = vswap;
                // int a = random(sd,srcc.layers()-1-sd);
                // int b_ = random(1,sd);
                // int c = random(0,1) ? 1 : -1;
                // // while(b_==0) b_ = random(-5,5);
                // int b = a+b_*c;
                
                int r = random(-sd,sd);
                int a = random(0,srcc.layers()-1);
                int b = a+r;
                b = constrain(b,0,srcc.layers()-1);
                if(a==b) continue;
                dstMutex.lock();
                auto &ba = *std::next(srcc.dst.comp.lil.begin(),a);
                auto &bb = *std::next(srcc.dst.comp.lil.begin(),b);
                auto x = srcc.eraseLine3(ba.layerIdx);
                auto y = srcc.eraseLine3(bb.layerIdx);
                srcc.drawLine3(x.start,x.end,*x.p_wire,y.layerIdx);
                srcc.drawLine3(y.start,y.end,*y.p_wire,x.layerIdx);
                // printf("%i %i  %i %i\n",a,b,x.layerIdx,y.layerIdx);
                dstMutex.unlock();
            }
        }

        //=============================================================
        // random move
        //=============================================================
        {
            PROFILE_SCOPE("move");
            extern int vmove;
            for(int i=0;i<vmove;i++)
            {
                
                dstMutex.lock();
                int l = random(0,srcc.layers()-1);
                auto ba = *std::next(srcc.dst.comp.lil.begin(),l);
                auto q = srcc.eraseLine3(ba.layerIdx);
                int dd = 1;
                int x=0,y=0;
                while(x==y){
                    int dx = random(-dd,dd);
                    int dy = random(-dd,dd);
                    x = q.start+dx;
                    y = q.end+dy;
                    while(x<0) x += NAILSCNT;
                    while(y<0) y += NAILSCNT;
                    x = x%NAILSCNT;
                    y = y%NAILSCNT;
                }
                srcc.drawLine3(x,y,*q.p_wire,ba.layerIdx);
                // printf("%i %i  %i %i\n",a,b,x.layerIdx,y.layerIdx);
                dstMutex.unlock();
            }
        }

        {
            PROFILE_SCOPE("cloning dst");
            dstMutex.lock();
            srcc.dst_ = srcc.dst.img.clone();
            // srcc.dst_ = srcc.dst2.clone();
            dstMutex.unlock();
        }
        //=============================================================
        // graph
        //=============================================================
        // graphPlot(graph1,graph2);
    }
  }
  printf("main break\n");
  return 0;
}
