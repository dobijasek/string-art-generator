#include "lineAA.h"
#include <opencv2/opencv.hpp>
#include "opencvex.h"
#include "pool.h"
int thickness = 1000;
int vwide = 1000;
float wide = 1000;
int vgc = 1000;
int vsub = 3;
int vimin = 0;
int mx = 0;
int my = 0;
int vlines = 16;
int vspeed = 10000;
int vssmode = SS_GRID;
int vrange = 500;
cv::Mat test;

void mouse_callback(int  event, int  x_, int  y_, int  flag, void *param)
{
    if (event == EVENT_MOUSEMOVE) {
        mx = x_;
        my = y_;
    }
}

bool inRange(const cv::Mat &m, int x, int y){
    return x>=0&&x<m.cols&&y>=0&&y<m.rows;
}

void blend8u(cv::Mat &m, int x, int y, double val, double alpha){
    pixel8u(m,x,y) = pixel8u(m,x,y)*(1.0-alpha) + val*(alpha);
}

void draw(std::vector<std::vector<Pixel>> lines){
    for(auto &line : lines){
        for(auto &px : line){
            if(inRange(test,px.x,px.y)){
                blend8u(test,px.x,px.y,255.0,px.alpha);
            }
        }
    }
}

int64_t millis(){
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return milliseconds;
}

#include <chrono>
int main(){
    // thickness = 3000;
    // vwide = 1;
    // namedWindow("GUI", WINDOW_AUTOSIZE );
    
    test.create(610,610,CV_8U);
    test = 0;
    imshow("test",test);
    
    createTrackbar("thick","test",&thickness,10000);
    createTrackbar("wide","test",&vwide,10000);
    createTrackbar("sub","test",&vsub,32);
    createTrackbar("ssmode","test",&vssmode,SS_MODES);
    createTrackbar("range","test",&vrange,2000);
    createTrackbar("gc","test",&vgc,50000);
    createTrackbar("lines","test",&vlines,64);
    createTrackbar("imin","test",&vimin,10000);
    createTrackbar("speed","test",&vspeed,10000);

    setMouseCallback("test", mouse_callback);
    while(1){
        double t = thickness/1000.0;
        wide = vwide/1000.0;
        wide = wide==0 ? t/sqrt(2) : wide;
        double gc = mapf(vgc,0,5000,0,5);
        test = 0;
        double imin = vimin/10000.0;
        double speed = vspeed/10000.0;
        double range = vrange/1000.0;
        

        // draw(10,30,10,380,t,wide,vsub,gc); // pionowo
        // draw(30,200,380,201,t,wide,vsub,gc); // prawie poziomo
        // draw(300,10,10,400,t,wide,vsub,gc);
        // draw(10,10,300,400,t,wide,vsub,gc);
        // draw(30,10,380,50,t,wide,vsub,gc);
        // draw(30,10+10,380,50+10,1.0,sqrt(2),vsub,1.0); // prawie poziomo
        // draw(250,250,x,y,t,wide,vsub,gc);
        
        double time = millis()/1000.0;
        double f = 0.02*speed;
        int divs = vlines;
        double radius = 300;
        double center = 305;
        std::vector<std::vector<Pixel>> que;
        std::mutex m;
        for(int i=0;i<divs;i++){
            TASKER.push({[&,i](){
                double q = 2.0*M_PI*f*time + 2.0*M_PI/divs/2.0*i;
                double amplitude = radius;;
                double xx = cos(q)*amplitude;
                double yy = sin(q)*amplitude;
                double x1 = center+xx;
                double y1 = center+yy;
                double x2 = center-xx;
                double y2 = center-yy;
                auto out = generateSmoothLine(x1,y1,x2,y2,t,wide,vsub,gc,imin,vssmode,range);
                m.lock();
                    que.push_back(out);
                m.unlock();
            }});
        }
        TASKER.wait();
        draw(que);
        // printf("%f\n",time);
        
        
        imshow("test",test);
        waitKey(1);
    }
}