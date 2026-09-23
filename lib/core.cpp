#include "core.h"
#include "util.h"
#include "line.h"
#include "score.h"
#include "pool.h"
#include "pair.h"
#include "opencvex.h"
#include "nn.h"
#include "score.h"
#include "Instrumentor.h"

using namespace cv;

extern float learnRate;
extern float decayRate;
extern int useBoost;
extern int fast;
extern int point_gap;
extern int w_val;

std::vector<double> core_t::getColorsFromSrc(int wires){
  std::vector<double> color(wires,255);
  for(int wireIdx=0;wireIdx<wires;wireIdx++){
    for(int pixel=0;pixel<w*h;pixel++){
      int val = src.data[pixel];
      if(wireIdx%wires==0){
        if(val<color.at(wireIdx)){
          color.at(wireIdx) = val;
        }
      } else {
        if(val<color.at(wireIdx) && val>color.at(wireIdx-1)){
          color.at(wireIdx) = val;
        }
      }
    }
  }
  return color;
}
// #include <filesystem>
void core_t::init(std::string filename, int Nails, int Sides, int Wires){
    sides = Sides;
    //===================================================
    printf("image: %s\n",filename.c_str());
    src = imread((char *)filename.c_str(), IMREAD_GRAYSCALE);
    w = src.cols;
    h = src.rows;
    if(w==0&&h==0){
      printf("blad ladowania pliku\n");
      exit(66);
      // terminate();
    }
    raw = src.clone();
    auto color = getColorsFromSrc(Wires);
    dst.init(Nails,Sides,color,w,h);
    dst2 = dst.img.clone();
    dstDone = dst.img.clone();
    used.create(dst.nail.size(),dst.nail.size(),CV_8U); used = 0;
    reset(0);
    createMask();
    
    src = src&mask;
    // line_init();
    dst_ = dst.img.clone();
    printf("c2nearestIdx\n");
    for(int j=0;j<256;j++){
      int mi = 0;
      float mv = FLT_MAX;
      for(auto &cc : dst.wire){
        float v = abs(j-cc.color);
        if(v<=mv){
          mv = v;
          mi = cc.index;
        }
      }
      c2nearestIdx[j] = mi;
    }
}

void core_t::reset(int Bval){
    used = 0;
    lineIdx = 0;
    hist.clear();
    dst.reset(Bval);
}

static void constrain(cv::Mat &m,float l, float h){
  for(int i=0;i<m.cols*m.rows;i++){
    pixel32fpx(m,i) = constrain(pixel32fpx(m,i),l,h);
  }
}

void core_t::drawLine(best_t &best){
    drawLine(best.start, best.end, best.wire->index);
    hist.push_back(best);
}

int core_t::getLineId(int start, int end){
  return getNailsIndex(start,end,dst.nail.size());
}

line_c& core_t::getLine(int start, int end){
  return getLine(getLineId(start,end));
}

line_c& core_t::getLine(int lineId){
  return line.at(lineId);
}

scoreResult_t core_t::getLineScoreFromLayer(int layerIdx){
  auto &hist_ = getHist(layerIdx);
  auto color_ = std::vector<double>{hist_.p_wire->color,0,0,0};
  auto sr = getLineScoreqq(color_,hist_.start,hist_.end,layerIdx,layerIdx).at(0);
  hist_.sr = sr;
  // printf("%f %f\n",sr.score,sr.score2);
  return sr;
}

lil_t& core_t::getHist(int layerIdx){
  return *std::next(dst.comp.lil.begin(),layerIdx);
}

std::vector<double> core_t::getColors(){
  std::vector<double> colors;
  for(auto &w : dst.wire){
    colors.push_back(w.color);
  }
  return colors;
}

void core_t::drawLine3(int start, int end, wire_t &wr, int layerIdxDraw){
  line_c &line = getLine(start, end);
  // auto lpixel = line.pixelLoad();
  // line.pixelSave(lpixel);
  int usedIdx = getNailsIndex(start,end,dst.nail.size());
  auto l = dst.comp.insert(start, end, usedIdx, line, wr, layerIdxDraw);
  double cw = dst.wire.at(wr.index).color;
  auto qq = line.pixelLoad();
  for(auto &px : qq){
    int x = px.x;
    int y = px.y;
    auto &cp = dst.comp.at(y*w+x);
    // li_t q(&l.layerIdx,&l.drop);
    cp.insert(l,cw,px.intensity);
    pixel32f(dst.img,x,y) = cp.color();
    // pixel32f(dst2,x,y) = cp.color2();
  }
  pixel8upx(used,getNailsIndex(start,end,dst.nail.size())) = 255;
}

lil_t core_t::eraseLine3(int layerIdxErase){
  lil_t hist = getHist(layerIdxErase);
  line_c &line = *hist.p_line;
  for(auto &px : line.pixelLoad()){
    int x = px.x;
    int y = px.y;
    computePixel_t &cp = dst.comp.at(y*w+x);
    cp.erase(layerIdxErase);
    pixel32f(dst.img,x,y) = cp.color();
    // pixel32f(dst2,x,y) = cp.color2();
  };
  dst.comp.erase(layerIdxErase);
  // line.pixelClear();
  pixel8upx(used,getNailsIndex(hist.start,hist.end,dst.nail.size())) = 0;
  return hist;
}

extern int thruLineMaskEnable;
void core_t::drawLine(int start, int end, int wireIdx){
  dst.drawLine(start,end,wireIdx);
  // thruLineMaskEnable = 0;
  // thruLineMaskEnable = 1;
  pixel8upx(used,getNailsIndex(start,end,dst.nail.size())) = 255;
  lineIdx++;
  return;
}

float getDistance(float xa, float ya, float xb, float yb){
  float dx = xa-xb;
  float dy = ya-yb;
  return sqrt(dx*dx+dy*dy);
}

void core_t::createMask(){
    printf("createMask\n");
    mask.create(src.size(),CV_8U);       mask = 0;
    maskDense.create(src.size(),CV_16U); maskDense = 0;
    float xc = (w+1)/2.0;
    float yc = (h+1)/2.0;
    float limit = 0.95;
    float rmax = getDistance(xc,yc,w,yc)*limit; // FIXME: tylko koło
    // printf("%f %i %i %f %f\n",rmax,w,h,xc,yc);
    mask = 255;
    cv::Mat maskTemp = mask.clone();
    maskTemp = 0;
    core_t &c = *this;
    thruPairAll(dst.nail.size(),[xc,yc,limit,rmax,&c,&maskTemp](int start, int end,int usedIdx){
      thruLine(start,end,c.dst.nail,[xc,yc,limit,rmax,&c,&maskTemp](int x, int y, double intensity){
        float r = getDistance(xc,yc,x,y);
        if(limit<1 && r>=rmax) return;
        pixel8u(maskTemp,x,y) = 255;
        pixel16u(c.maskDense,x,y) += 1;
      });
    });
    mask = maskTemp.clone();
    double min,max;
    minMaxIdx(maskDense,&min,&max,nullptr,nullptr,mask);
    printf("maskDense %lf %lf\n",min,max);
    dst.weight = 0;
    for(int i=0;i<w*h;i++){
      // if(pixel8upx(mask,i)==0) continue;
      int den = pixel16upx(maskDense,i);
      double val = mapf(den,min,max,1,0.05);
      if(den<min) val = 0.0;
      // printf("%f %i\n",val,den);
      dst.weight.at<double>(i) = val;
    }
    // imshowN16("masekDense",maskDense);
    // imshowN16("mask",dst.weight);
    // waitKey(2000);
}

void core_t::line_init(){
  printf("line_init\n");
  int i = 0;
  std::mutex m;
  line.resize(mask.cols*mask.rows);
  thruPairAll(dst.nail.size(),[&](int start, int end,int usedIdx){
    TASKER.push({[this,usedIdx,start,end,&i,&m](){
      line.at(usedIdx).init(src,dst.img,mask,dst.nail,start,end);
      m.lock();
      i++;
      if(i%1000==0) printf("%i\n",i);
      m.unlock();
    }});
    
  });
  TASKER.wait();
}

scoreResult_t core_t::getLineScore_(int color, int lineIdx, int layerIdx){
  PROFILE_FUNCTION();
  layerIdx = layerIdx==-1 ? INT_MAX-1 : layerIdx;
  return ::getLineScore(color,line.at(lineIdx),src,dst.img,dst.layerNum,layerIdx); // TODO: do wywalenia
}