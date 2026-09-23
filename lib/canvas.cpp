#include "canvas.h"
#include "score.h"
#include "line.h"
#include "opencvex.h"
#include "Instrumentor.h"

cv::Mat canvas_t::getDenseq(){
    cv::Mat denseq(h,w,CV_8U);
    denseq = 0;
    for(int i=0;i<w*h;i++){
        pixel8upx(denseq,i) = comp.at(i).dense();
    }
    return denseq;
}

void canvas_t::init(int nails, int sides, const std::vector<double> &color, int W, int H){
    w = W;
    h = H;
    nail = generateNails(nails,sides,w,h);
    for(int wireIdx=0;wireIdx<color.size();wireIdx++){
        wire.push_back({color.at(wireIdx),wireIdx});
    }
    img.create(h,w,CV_32F);
    weight.create(h,w,CV_64F);
    dense.create(h,w,CV_8U); // TODO: int32?
    cover.create(h,w,CV_8U);
    layerNum.create(h,w,CV_16U);
    img.create(h,w,CV_32F);
    dense.create(h,w,CV_8U); // TODO: int32?
    cover.create(h,w,CV_8U);
    layerNum.create(h,w,CV_16U);
    reset(0);
}
extern core_t srcc;
void canvas_t::reset(int Bval){
    bval = Bval;
    img = bval;
    // weight = 1;
    dense = 0;
    cover = 0;
    layerNum = 0;
    comp.clear();
    comp.init(srcc.src,weight);
    layerIdx = 0;
    for(auto &Wire : wire){
        Wire.nail = 0;
        Wire.side = 0;
    }
}

void canvas_t::drawLine(int start, int end, int wireIdx){
    drawLine(img,start,end,wireIdx);
}

extern core_t srcc;
void canvas_t::drawLine(cv::Mat &m, int start, int end, int wireIdx){
    PROFILE_FUNCTION();
    layerIdx++;
    thruLine(start,end,nail,[&](int x, int y,double intensity){
        pixel32f(m,x,y) = wire.at(wireIdx).color;
        pixel16u(layerNum,x,y) = layerIdx;
        pixelInc8U(dense,x,y,1);
    });
    wire.at(wireIdx).nail = end;
}

void canvas_t::eraseLine(int start, int end, int LI){
    PROFILE_FUNCTION();
    thruLineAA(start,end,nail,[&](int x, int y,double intensity){
        auto &cp = comp.at(y*w+x);
        cp.erase(LI);
        // pixel16u(layerNum,x,y) = cp.layerIdx();
        pixel32f(img,x,y)      = cp.color();
        // pixel8u(dense,x,y)     = cp.dense();
    });
    comp.erase(LI);
}