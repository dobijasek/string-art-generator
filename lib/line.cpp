#include "line.h"
#include "opencvex.h"

void swap_coord( int *a, int *b ){
    int t = *a;
    *a = *b;
    *b = t;
}

#include "core.h"
extern core_t srcc;
int thruLineMaskEnable = 0;

void thruLine(int x1,int y1,int x2,int y2,tl_t& f){
    // przerobić tak aby pointery zawsze rosły
    bool steep = abs( y2-y1 )>abs( x2-x1 );
    if( steep ){
        swap_coord( &x1, &y1 );
        swap_coord( &x2, &y2 );
    }

    if( x1>x2 ){
        swap_coord( &x1, &x2 );
        swap_coord( &y1, &y2 );
    }

    int32_t dx = x2-x1;
    int32_t dy = abs( y2-y1 );

    int32_t err = dx>>1;
    int32_t ystep = y1<y2 ? 1 : -1;
    int32_t xs = x1;
    int32_t dlen = 0;

    if( steep ){
        for( ; x1<=x2; x1++ ){
            dlen++;
            err -= dy;
            if( err<0 ){
                err += dx;
                for(int i=0;i<dlen;i++){
                    if(thruLineMaskEnable==0 || pixel8u(srcc.mask,y1,xs+i)!=0) f(y1,xs+i,1);
                }
                dlen = 0;
                y1 += ystep;
                xs = x1+1;
            }
        }
        for(int i=0;i<dlen;i++){
            if(thruLineMaskEnable==0 || pixel8u(srcc.mask,y1,xs+i)!=0) f(y1,xs+i,1);
        }
    } else{
        for( ; x1<=x2; x1++ ){
            dlen++;
            err -= dy;
            if( err<0 ){
                err += dx;
                for(int i=0;i<dlen;i++){
                    if(thruLineMaskEnable==0 || pixel8u(srcc.mask,xs+i,y1)!=0) f(xs+i,y1,1);
                }
                dlen = 0;
                y1 += ystep;
                xs = x1+1;
            }
        }
        for(int i=0;i<dlen;i++){
            if(thruLineMaskEnable==0 || pixel8u(srcc.mask,xs+i,y1)!=0) f(xs+i,y1,1);
        }
    }
}

void thruLineqqq(int x1, int y1, int x2, int y2, const std::function<void(int,int)>& f){
  // rezultaty zmieniają się nieznacznie w zależności od miejsca użycia
  if(abs(y1-y2)>abs(x1-x2)){
    float slope = (float)(x2-x1) / (float)(y2-y1);
    int starty = y1<y2?y1:y2;
    int endy = y2>y1?y2:y1;
    for(int y = starty; y < endy; y++){
      int x = x1 + round((float)(y-y1) * slope);
      f(x,y);
    }
  } else {
    float slope = (float)(y2-y1) / (float)(x2-x1);
    int startx = x1<x2?x1:x2;
    int endx = x2>x1?x2:x1;
    for(int x = startx; x < endx; x++){
      int y = y1 + round((float)(x-x1) * slope);
      f(x,y);
    }
  }
}
#include "lineAA.h"
void thruLine(int start, int end,const std::vector<nail_t> &nail,tl_t& f){
  int x1 = nail.at(start).x;
  int y1 = nail.at(start).y;
  int x2 = nail.at(end).x;
  int y2 = nail.at(end).y;
  thruLine(x1,y1,x2,y2,f);
}

void thruLineAA(int start, int end,const std::vector<nail_t> &nail,tl_t& f){
  int x1 = nail.at(start).x;
  int y1 = nail.at(start).y;
  int x2 = nail.at(end).x;
  int y2 = nail.at(end).y;

  double thickness = randomf(1.3,4);
  double wide = random(0.9,1.5);
  wide = 1.4;
  double vsub = 2;
  double gc = randomf(1.7,1.9);
  double imin = 1.0/255.0*32;
  double vssmode = SS_RANDOM;
  double range = 0.5;
  if(1){
    thickness = 2.2;
    wide = 1.5;
    vsub = 3;
    gc = 2;
    imin = 1.0/255.0*32;
    vssmode = SS_GRID;
    range = 0.5;
  }
  int DIMX = 544;
  int DIMY = DIMX;
  auto ll = generateSmoothLine(x1,y1,x2,y2,thickness,wide,vsub,gc,imin,vssmode,range);
  for(auto l : ll){
    if(l.x<0 || l.y<0 || l.x>=DIMX || l.y>=DIMY) continue;
    f(l.x,l.y,l.alpha);
  }
}

int getLineLen(int start, int end,const std::vector<nail_t> &nail){
  int lineLen=0;
  thruLine(start,end,nail,[&](int x, int y, double intensity){
    lineLen++;
  });
  return lineLen;
}

int getLineLenMask(int start, int end, std::vector<nail_t> &nail, Mat *mask){
  int lineLen=0;
  thruLine(start,end,nail,[&](int x, int y, double intensity){
    lineLen += pixel8u(*mask,x,y)?1:0;
  });
  return lineLen;
}

int getLineLen2(int start, int end, std::vector<nail_t> &nail){
  int xd = abs(nail[start].x-nail[end].x);
  int yd = abs(nail[start].y-nail[end].y);
  float q = sqrt(xd*xd + yd*yd);
  return q;
}

void getLine(Mat *m, int *arr, int x1, int y1, int x2, int y2){
  int i = 0;
  thruLine(x1,y1,x2,y2,[&](int x,int y, double intensity){
    arr[i] = pixel8u(*m,x,y);
    i++;
  });
}

float getLineMean(Mat *m, int start, int end, std::vector<nail_t> &nail){
  int cnt = 0;
  uint32_t sum = 0;
  thruLine(start,end,nail,[&](int x,int y, double intensity){
    sum += pixel8u(*m,x,y);
    cnt++;
  });
  return (float)sum/(float)cnt;
}

void getLineStats(Mat *m, Mat *mask, int start, int end, std::vector<nail_t> &nail, float *min, float *max, float *mean, float *len){
  *min = FLT_MAX;
  *max = FLT_MIN;
  *mean = 0;
  *len = 0;
  int msk = 1;
  thruLine(start,end,nail,[&](int x,int y, double intensity){
    if(mask!=nullptr) msk = pixel8u(*mask,x,y);
    if(msk){
      float val = pixel32f(*m,x,y);
      *mean += val;
      *len += 1.0;
      if(val<*min) *min = val;
      if(val>*max) *max = val;
    }
  });
  *mean /= *len>0 ? *len : 1;
}

float getLineMeanf(Mat *m, int start, int end, std::vector<nail_t> &nail){
  int cnt = 0;
  uint32_t sum = 0;
  thruLine(start,end,nail,[&](int x,int y, double intensity){
    sum += pixel32f(*m,x,y);
    cnt++;
  });
  return (float)sum/(float)cnt;
}

#include "pair.h"
void line_c::init(cv::Mat &src,cv::Mat &dst,cv::Mat &mask
    ,std::vector<nail_t> &nail, int start, int end){
  srcp = &src;    
  s = start;
  e = end;
  np = &nail;
  mp = &mask;
  if(start==0&&end==0){
    printf("xxx\n");
  }
  len = getLineLenMask(start,end,nail,&mask);
  int l = 0;
  // score = FLT_MAX;
  
  len = l;
  // pixel.clear();
  pixel = pixelGenerate();
  std::sort(pixel.begin(),pixel.end(),[](linePixel_t &l, linePixel_t &r){return l.imgOffset<r.imgOffset;});
}

void line_c::pixelSave(std::vector<linePixel_t> pp){
  pixel = pp;
}

void line_c::pixelClear(){
  pixel.clear();
}

std::vector<linePixel_t> line_c::pixelLoad(){
  // return pixelGenerate();
  return pixel;
  // if(pixel.size()==0){
  //   return pixelGenerate();
  // } else {
  //   return pixel;
  // }
  // return pixel.size()==0 ? pixelGenerate() : pixel;
}

std::vector<linePixel_t> line_c::pixelGenerate(){
  std::vector<linePixel_t> px;
  thruLine(s,e,*np,[&](int x, int y,double intensity){
    if(pixelInRange(*srcp,x,y))
    if(pixel8u(*mp,x,y))
    {
      uint32_t offset = srcp->cols*y+x;
      uint32_t usedIdx = getNailsIndex(s,e,np->size());
      // double intensity = randomf(0.05,0.2);
      // double intensity = randomf(0.4,0.7);
      intensity = 0.6;
      scoreResult_t sr;
      sr.len = -1;
      px.push_back({(uint16_t)x,(uint16_t)y,(float)intensity,offset,usedIdx});
    }
  });
  return px;
}

void line_c::thruLine_(int start, int end, tl_t& f){
  for(auto p : pixel){
    f(p.x,p.y,1);
  }
}