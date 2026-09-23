#include "score.h"
#include "util.h"
#include "line.h"
#include "pool.h"
#include "opencvex.h"
#include "nn.h"
#include "Instrumentor.h"

int isBottom(int pixelMethod){
  int bottom = 0;
  if(pixelMethod==PIXEL_BOTTOM) bottom = 1;
  return bottom;
}

float gc(float x, float gamma){
  return pow(x/255.0,gamma);
}

extern core_t srcc;
scoreResult_t getLineScore(int color,line_c &line,const cv::Mat &src,const cv::Mat &dst,const cv::Mat &layer, int layerIdx){
  // PROFILE_FUNCTION();
  layerIdx += 1;
  float scoreSum = 0;
  float q = 1.0/255.0;
  q = 1;
  float cw = color;
  int l = 0;
  for(const auto &pt : line.pixelLoad()){
    int layerPt = pixel16u(layer,pt.x,pt.y);
    if(layerPt<layerIdx){
      float cs = pixel8u(src,pt.x,pt.y)*q;
      float cd = pixel32f(dst,pt.x,pt.y)*q;
      scoreSum += abs(cs-cw)-abs(cd-cw);
      // scoreSum += -abs(cs-cw)+abs(cd-cw);
      // scoreSum += cs;
      l++;
    } else {
      // scoreSum += -cw;
    }
  }
  // line.score = (l<=1) ? (FLT_MAX) : (scoreSum/(float)(l-1));
  line.score = (scoreSum)/line.len;
  // line.score = abs(scoreSum/l-cw);
  // printf("%f %f %f\n",scoreSum/l,cw,line.score);
  scoreResult_t sr;
  sr.score = line.score;
  sr.len = line.len;
  return sr;
}
extern int v0;
extern int v1;
#include "nn.h"
float pow(float x){
  return x*x;
}

scoreResult_t getScoreLine(int start, int end, core_t &c, int wireIdx, const scoreMode_t &method){
    // PROFILE_FUNCTION();
    int usedIdxx = getNailsIndex(start,end,c.dst.nail.size());
    float processedx = c.line.at(usedIdxx).len;
    if(processedx<=0){
      scoreResult_t sr;
      sr.score = FLT_MAX;
      sr.len = -1;
      return sr;
    }
    float cwx =  c.dst.wire.at(wireIdx).color;
    // float scorex = c.line.at(usedIdxx).getScore(wireIdx);
    scoreResult_t sr = c.getLineScore_(cwx,usedIdxx,-1);
    return sr;
}

double getImgErrorProcentage(Mat &src_, Mat &dst_, Mat &mask_, double q1){
  cv::Mat src,dst,mask;
  src_.convertTo(src,CV_64F,1,0);
  dst_.convertTo(dst,CV_64F,1,0);
  mask_.convertTo(mask,CV_8U,1,0);
  int l = src.rows*src.cols;
  double score = 0;
  uint32_t cnt = 0;
  for(int i=0;i<l;i++){
    if(pixel8upx(mask,i)==0) continue;
    score += pscoreTanh(pixel64fpx(src,i)/255.0,pixel64fpx(dst,i)/255.0,q1);
    cnt++;
  }
  return score;
  // return pow(score/(double)cnt,1.0/q1)*100.0;
}

double getImgErrorProcentage2(Mat &src_, Mat &dst_, Mat &mask_, double q1){
  cv::Mat src,dst,mask;
  src_.convertTo(src,CV_64F,1,0);
  dst_.convertTo(dst,CV_64F,1,0);
  mask_.convertTo(mask,CV_8U,1,0);
  int l = src.rows*src.cols;
  int cnt = 0;
  double score = 0;
  double sumA = 0;
  double meanA = 0;
  double sumB = 0;
  double meanB = 0;
  double A,B;
  for(int i=0;i<l;i++){
    if(pixel8upx(mask,i)==0) continue;
    A = pixel64fpx(src,i);
    B = pixel64fpx(dst,i);
    sumA = A;
    sumB = B;
    cnt++;
  }
  meanA = sumA/cnt;
  meanB = sumB/cnt;
  double cov = 0;
  double var = 0;
  double stdA = 0;
  double stdB = 0;
  for(int i=0;i<l;i++){
    if(pixel8upx(mask,i)==0) continue;
    A = pixel64fpx(src,i);
    B = pixel64fpx(dst,i);
    cov += (A-meanA)*(B-meanB);
    var += (A-meanA)*(A-meanA);
    stdA += (A-meanA)*(A-meanA);
    stdB += (B-meanB)*(B-meanB);
  }
  stdA = sqrt(stdA);
  stdB = sqrt(stdB);
  // score = cov/var;
  score = cov/(stdA*stdB);
  return score;
  // return pow(score/(double)cnt,1.0/q1)*100.0;
}

double getImgPaerson(Mat &src_, Mat &dst_, Mat &mask_, double q1){
  cv::Mat src,dst,mask;
  src_.convertTo(src,CV_64F,1,0);
  dst_.convertTo(dst,CV_64F,1,0);
  mask_.convertTo(mask,CV_8U,1,0);
  int l = src.rows*src.cols;
  Paerson__ p;
  for(int i=0;i<l;i++){
    if(pixel8upx(mask,i)==0) continue;
    double A = pixel64fpx(src,i);
    double B = pixel64fpx(dst,i);
    // p.add(i,A,B,1);
  }
  double score = p.compute();
  return score;
}

scoreResult_t getScorePixel(int x, int y, const core_t &c, int wireIdx, int pixelMethod){
  float score = 0;
  float len = 1;
  float processed = 0;
  switch(pixelMethod){
    case PIXEL_WOW:
    {
      float cw = c.dst.wire.at(wireIdx).color;
      float cs = pixel8u(c.src,x,y);
      float cd = pixel32f(c.dst.img,x,y);
      float sc = abs(cs-cw) - abs(cd-cw);
      score = sc;
      processed = 1;
      break;
    }
    case PIXEL_WOW2:
    {
      int cw = c.dst.wire.at(wireIdx).color;
      int cs = pixel8u(c.src,x,y);
      int cd = pixel32f(c.dst.img,x,y);
      float sc = (cs!=cw) - (cd!=cw);
      score = sc;
      processed = 1;
      break;
    }
    case PIXEL_BOTTOM:
    {
      if(pixel8u(c.dst.cover,x,y)!=0) break;
      int cw = c.dst.wire.at(wireIdx).color;
      int cs = pixel8u(c.src,x,y);
      score = abs(cs-cw);
      processed = 1;
      break;
    }
    case PIXEL_SSUM:
    {
      int cs = pixel8u(c.src,x,y);
      score = cs;
      processed = 1;
      break;
    }
    case PIXEL_DSUM:
    {
      int cd = pixel32f(c.dst.img,x,y);
      score = cd;
      processed = 1;
      break;
    }
    case PIXEL_ABSCSCW:
    {
      int cw = c.dst.wire.at(wireIdx).color;
      int cs = pixel8u(c.src,x,y);
      score = abs(cs-cw);
      processed = 1;
      break;
    }
    case PIXEL_ABSCDCW:
    {
      int cw = c.dst.wire.at(wireIdx).color;
      int cd = pixel32f(c.dst.img,x,y);
      score = abs(cd-cw);
      processed = 1;
      break;
    }
    case PIXEL_ABSCSCD:
    {
      int cs = pixel8u(c.src,x,y);
      int cd = pixel32f(c.dst.img,x,y);
      score = abs(cs-cd);
      processed = 1;
      break;
    }
    default:
    {
      printf("brak wsparcia dla pixelMethod=%i\n",pixelMethod);
      break;
    }
  }
  scoreResult_t sr;
  sr.score = score;
  sr.len = len;
  sr.processed = processed;
  return sr;
}