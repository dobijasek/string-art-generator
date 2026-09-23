#include "graph.h"
#include <algorithm>
#include <cmath>
#include "util.h"
#include "opencvex.h"
#include "gui.h"

graph::graph(int w, int h) : w(w), h(h)
{
    m.create(h,w,CV_8UC3);
}

static cv::Mat graph(500,1000,CV_8UC3);

void graphPlot(std::vector<double> &graph1,std::vector<double> &graph2){
    while(graph1.size()>graph.cols) graph1.erase(graph1.begin());
    while(graph2.size()>graph.cols) graph2.erase(graph2.begin());
    int cnt = min(graph1.size(),graph2.size());
    float min1 = *std::min_element(graph1.end()-cnt,graph1.end());
    float max1 = *std::max_element(graph1.end()-cnt,graph1.end());
    float min2 = *std::min_element(graph2.end()-cnt,graph2.end());
    float max2 = *std::max_element(graph2.end()-cnt,graph2.end());
    static float pmin1 = FLT_MAX;;
    static float pmax1 = FLT_MAX;;
    static float pmin2 = FLT_MAX;;
    static float pmax2 = FLT_MAX;;
    float l = min(min1,min2);
    float h = max(max1,max2);
    if(min1!=max1 && min2!=max2){
        graph = 0;
        for(int i=0;i<cnt;i++){
            int x = i;
            int y0 = round(mapf(0,l,h,graph.rows-1,0));
            int y1 = round(mapf(graph1.at(i),l,h,graph.rows-1,0));
            int y2 = round(mapf(graph2.at(i),l,h,graph.rows-1,0));
            if(y0>=0 && y0<graph.rows) pixelVec3b(graph,x,y0) = cv::Vec3b(64,64,64);
            if(y1!=y2){
                if(y1>=0 && y1<graph.rows) pixelVec3b(graph,x,y1) = cv::Vec3b(0,0,255);
                if(y2>=0 && y2<graph.rows) pixelVec3b(graph,x,y2) = cv::Vec3b(255,255,0);
            } else {
                if(y1>=0 && y1<graph.rows) pixelVec3b(graph,x,y1) = cv::Vec3b(32,32,32);
            }
        }
        imshow_("graph",graph);
        char filename[1000];
        sprintf(filename,"%.2f %.2f    %.2f %.2f\n",min1,max1,min2,max2);
        if(min1!=pmin1 || max1!=pmax1 || min2!=pmin2 || max2!=pmax2){
            pmin1 = min1;
            pmax1 = max1;
            pmin2 = min2;
            pmax2 = max2;
            // printf("%s",filename);
        }
        // cv::displayOverlay("graph",filename,0);
    }
}