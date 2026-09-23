#pragma once
#include <stdio.h>
#include <fstream>
#include "best.h"
#include <opencv2/opencv.hpp>
#include "intersect.h"
extern core_t srcc;
char oo[1000000];
char oo2[1000000];

inline void saveHistory(int lineIdx, int history_len, best_t *history){
  int ooi = 0;
  int len = lineIdx<srcc.hist.size() ? lineIdx : srcc.hist.size();
  for(int i=0;i<len;i++){
    int id = (lineIdx-len+i)%history_len;
    best_t *b = &history[id];
    int s = b->start+1;
    int e = b->end+1;
    ooi += sprintf(oo+ooi,"%i %3i%s %3i%s\n",b->wire->index, s>207?s-1:s , s==207?"X":"" ,e>207?e-1:e ,e==207?"X":"" );
  }
  std::ofstream rawData;
  rawData.open ("rawData.txt");
  rawData.write(oo,strlen(oo));
  rawData.close();

  Mat historyUse(1,history_len,CV_8U);
  historyUse = 0;
  int drawed = 0;
  int ooi2 = 0;
  while(1){
    int len = lineIdx<srcc.hist.size() ? lineIdx : srcc.hist.size();
    int pc=-1;
    int drawedDelta = 0;
    struct Pointt p2;
    struct Pointt q2;

    for(int i=0;i<len;i++){
      int id = (lineIdx-len+i)%history_len;
      best_t *b = &history[id];
      int s = b->start;
      int e = b->end;
      int c = b->wire->color;
      if(historyUse.data[id]==1){
        // pomiń użyte elementy
        continue;
      }
      struct Pointt p1 = {srcc.dst.nail.at(s).x,srcc.dst.nail.at(s).y};
      struct Pointt q1 = {srcc.dst.nail.at(e).x,srcc.dst.nail.at(e).y};
      if(pc>=0 && c!=pc && doIntersect(p1,q1,p2,q2)){
        // jeśli obecny kolor nie jest pierwszym elementem sekwencji
        // jeśli kolor obecnego elementu jest inny od poprzedniego
        // sprawdź czy linie się przecinają
        break;
      }
      if(pc<0 || c==pc){
        // jeśli obecny kolor jest pierwszym elementem sekwencji
        // lub obecny kolor zgadza się z poprzednim
        historyUse.data[id] = 1;
        pc = c;
        p2 = p1;
        q2 = q1;
        s++;
        e++;
        ooi2 += sprintf(oo2+ooi2,"%i %3i%s %3i%s\n",b->wire->index, s>207?s-1:s , s==207?"X":"" ,e>207?e-1:e ,e==207?"X":"" );
        drawed++;
        drawedDelta++;
      }
    }
    if(drawed+1==srcc.hist.size()) break;
    if(drawedDelta==0) printf("coś poszło nie tak %i %i %i %i",drawed,drawedDelta,pc,lineIdx);
  }
  std::ofstream rawData2;
  rawData2.open ("rawData2.txt");
  rawData2.write(oo2,strlen(oo2));
  rawData2.close();
}