#pragma once
#include <cmath>
#include <vector>

struct pers
{
  double *A;
  double *B;
};
class Paerson__2
{
  public:
  int w;
  int h;
  double p = 1;
  int r = 0;
  std::vector<double> A;
  std::vector<double> B;
  std::vector<std::vector<pers>> Q;
  int idx(int x, int y);
  void init(int w_, int h_);

  void clear();
  void add(int idx, double A, double B,double W);
  void update(int idx, double oA, double oB, double nA, double nB,double oW, double nW);
  double compute();
};