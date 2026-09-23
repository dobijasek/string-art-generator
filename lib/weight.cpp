#include "weight.h"
#include "core.h"
#include "opencvex.h"
#include "line.h"
#include "util.h"
#include "nn.h"

using namespace cv;
extern Mat wd[];
extern Mat wl[];
extern core_t srcc;
extern float decayRate;
extern Mat wcb[][4];
extern Mat wbi[];
extern float learnRate;
extern int bval;

inline void blendq(Mat &weight,Mat &cost,Mat &out,float alpha){
    float *Weight = (float*)weight.data;
    float *Cost = (float*)cost.data;
    float *Out = (float*)out.data;
    int len = weight.rows*weight.cols;
    for(int i=0;i<len;i++){
      Out[i] = Weight[i]*alpha + Cost[i]*(1-alpha);
      // Out[i] = Cost[i]?Cost[i]:Weight[i]*alpha;
      // Out[i] = Cost[i];
      // Out[i] = Weight[i]-abs(Cost[i])*alpha;
    }
}

inline void blendq2(Mat &weight,Mat &cost,Mat &out,float alpha){
    float *Weight = (float*)weight.data;
    float *Cost = (float*)cost.data;
    float *Out = (float*)out.data;
    int len = weight.rows*weight.cols;
    for(int i=0;i<len;i++){
    //   Out[i] = Weight[i]*alpha + Cost[i]*(1-alpha);
      // Out[i] = Cost[i]?Cost[i]:Weight[i]*alpha;
      // Out[i] = Cost[i];
      Out[i] = Weight[i]-abs(Cost[i])*alpha;
    }
}

void constrain(Mat &m, float min, float max){
  int len = m.rows*m.cols;
  float *f = (float*)m.data;
  for(int i=0;i<len;i++){
    f[i] = f[i]<min ? min : f[i]>max ? max : f[i];
  }
}

extern int wireIdxSlider;
#include "gui.h"
// normalizacja dense
void weightUpdate9(){
    // int linesPerStack = srcc.hist.size()/stackSize;
    // // for(int i=1;i<srcc.hist.size();i++){
    // for(int stackIdx=0;stackIdx<stackSize;stackIdx++){
    //     // if(stackIdx!=stackSelect) continue;
    //     Mat lt,lb;
    //     inRange(srcc.dst.layer,stackIdx*linesPerStack+1,stackIdx*linesPerStack+linesPerStack+1,lt);
    //     srcc.dst.layerDense = lt&(srcc.dst==srcc.src);
    //     if(stackIdx==stackSelect) imshow_("test",srcc.dst.layerDense);
    //     // srcc.dst.layerDense = lt;
    //     float alpha = learnRate;
    //     float beta = 1-alpha;
        Mat ltf = srcc.dst.dense.clone();
        ltf.convertTo(ltf,CV_32F,1.0/255.0);
        // ltf = 1-ltf;
        // srcc.pixelWeight = srcc.pixelWeight*(1-learnRate) + ltf*(learnRate);
        // srcc.weightSrc = srcc.weightSrc - ltf*(learnRate);
        // srcc.weightSrc = srcc.weightSrc + 1*decayRate;
        // srcc.pixelWeight = srcc.pixelWeight*(1-decayRate) + 1*decayRate;
    //     // break;
    //     // printf("%i   %i %i %i\n",i,srcc.lineIdx,srcc.hist.size(),linesPerStack);
    //     // break;
    // }
    // constrain(srcc.weightSrc,0,100);
}

void weightUpdate10(){
    // int linesPerStack = srcc.hist.size()/stackSize;
    // // for(int i=1;i<srcc.hist.size();i++){
    // for(int stackIdx=0;stackIdx<stackSize;stackIdx++){
    //     // if(stackIdx!=stackSelect) continue;
    //     Mat lt,lb;
    //     inRange(srcc.dst.layer,stackIdx*linesPerStack+1,stackIdx*linesPerStack+linesPerStack+1,lt);
    //     srcc.dst.layerDense = lt&(srcc.dst==srcc.src);
    //     if(stackIdx==stackSelect) imshow_("test",srcc.dst.layerDense);
    //     // srcc.dst.layerDense = lt;
    //     float alpha = learnRate;
    //     float beta = 1-alpha;
        // Mat coverf;
        // srcc.dst.cover.convertTo(coverf,CV_32F,1.0/255.0);
        // srcc.pixelWeight += coverf;
        // srcc.pixelWeight = srcc.pixelWeight-(1-coverf)*0.1;
        // srcc.weightSrc = srcc.weightSrc*(1-learnRate) + (coverf)*learnRate;
        // srcc.weightSrc = srcc.weightSrc*(1-decayRate) + 1*decayRate;
        // constrain(srcc.pixelWeight,0.1,1);
    //     // break;
    //     // printf("%i   %i %i %i\n",i,srcc.lineIdx,srcc.hist.size(),linesPerStack);
    //     // break;
    // }
}
 extern int bval_;
// cv::Mat historyGetImage(int A, int B){

// }
extern int lines;
void weightUpdate11(){
    // std::vector<best_t> h = srcc.hist;
    // srcc.gradient = loss(srcc.dst.img,srcc.src);
    // // imshow_("222",srcc.gradient,1);
    // for(int lineIdx=h.size()-1;lineIdx>=0;lineIdx--){
    //     srcc.setStack(lines,lineIdx);
    //     // srcc.drawLineBP(h.at(lineIdx));
    //     srcc.line.at(h.at(lineIdx).usedIdx).BP(srcc.gradient,h.at(lineIdx));
    // }
    // // imshow_("qqq",srcc.dst.dense,1);
    // // return;
    // static int cnt = 0; cnt++;
    // int batchSize = 1;
    // float lr = -learnRate/1000;
    // lr = 1;
    // if(cnt%batchSize==0){ // batch
    //     for(auto w : srcc.dst.wire){
    //         for(auto &w_ : w.weight_){
    //             w_->weightSrc   += lr*w_->weightSrcDelta/batchSize;
    //             // w_->weightDst   += lr*w_->weightDstDelta/batchSize;
    //             w_->weightDense += lr*w_->weightDenseDelta/batchSize;
    //             w_->biasSrc     += lr*w_->biasSrcDelta/batchSize;
    //             // w_->biasDst     += lr*w_->biasDstDelta/batchSize;
                
    //             w_->weightSrc    = w_->weightSrc*(1.0-decayRate)   + 0.05*(decayRate);
    //             // w_->weightDst    = w_->weightDst*(1.0-decayRate)   - 0.5*(decayRate);
    //             w_->weightDense  = w_->weightDense*(1.0-decayRate) + 0.0*(decayRate);
    //             w_->biasSrc      = w_->biasSrc*(1.0-decayRate)     + 0.0*(decayRate);
    //             // w_->biasDst      = w_->biasDst*(1.0-decayRate)     + 0.0*(decayRate);

    //             float limitW = 2.0;
    //             float limitB = 2.0;
    //             float limitD = 2.0;
    //             constrain(w_->weightSrc,   -0,limitW);
    //             // constrain(w_->weightDst,   -limitW,0);
    //             constrain(w_->weightDense, -0,limitW);
    //             constrain(w_->biasSrc,     -limitB,limitB);
    //             // constrain(w_->biasDst,     -limitB,limitB);
    //         }
    //     }
    //     for(auto w : srcc.dst.wire){
    //         for(auto &w_ : w.weight_){
    //             w_->weightSrcDelta   = 0;
    //             w_->weightDstDelta   = 0;
    //             w_->weightDenseDelta = 0;
    //             w_->biasSrcDelta     = 0;
    //             w_->biasDstDelta     = 0;
    //         }
    //     }
    // }
}

std::string weightGetFilename(int stackIdx, int wireIdx){
    char filename[100];
    sprintf(filename,"project/wc_%i_%2i.tiff",wireIdx,stackIdx);
    return filename;
}
const int rw = 0;
extern float score_;
extern int iter_;
void weightSave(){
    // if(!rw) return;
    // for(int stackIdx=0;stackIdx<stackSize;stackIdx++){
    //     // char filename[100]; sprintf(filename,"project/wbi_%i.tiff",stackIdx);
    //     // imwrite(filename,wbi[stackIdx]);
    //     for(int wireIdx=0;wireIdx<4;wireIdx++){
    //         {
    //             // std::string filename = weightGetFilename(stackIdx,wireIdx);
    //             // imwrite(filename,stackWireWeight[stackIdx][wireIdx]);
    //         }
    //         {
    //             char filename[100]; sprintf(filename,"project/dst/%05i_%.3f.jpg",iter_,score_);
    //             imwrite(filename,srcc.dst.img);
    //         }
    //         {
    //             char filename[100]; sprintf(filename,"project/indexWeight/%i_%03i.TIFF",wireIdx,stackIdx);
    //             imwrite(filename,stackWireIweight[stackIdx][wireIdx]);
    //         }
    //     }
    // }
}

void weightLoad(){
}