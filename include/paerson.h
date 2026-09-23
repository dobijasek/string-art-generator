#pragma once
#include <cmath>
#include "paerson2.h"

class Paerson__
{
  public:
  int N = 0;
  double SA = 0;
  double SB = 0;
  double SW = 0;
  double SQ = 0;
  double SA2 = 0;
  double SA3 = 0;
  double SA4 = 0;
  double SB2 = 0;
  double SB3 = 0;
  double SB4 = 0;
  double SAB = 0;
  double SDAB = 0;
  double SDAB2 = 0;
  double qq();
  double qq2();
  double p = 1;
  std::vector<int> mask;
  void init(int w, int h);
  void add(const Paerson__ &pa);
  void sub(const Paerson__ &pa);
  void clear();
  void add(int idx, double A, double B,double W);
  void update(int idx, double oA, double oB, double nA, double nB,double oW, double nW);
  void print();
  double compute2();
  double compute3() const;
  double compute4();
  double compute5();
  double compute();
};