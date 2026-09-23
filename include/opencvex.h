#pragma once
#include <opencv2/opencv.hpp>
#include "queue.h"
using namespace cv;

inline void imshowN8(const String &winname,const  Mat &mat){
  Mat m = mat.clone();
  normalize(m,m,0,255,NORM_MINMAX);
  imshow(winname,m);
}

inline void imshowAs8U(const String &winname,const  Mat &mat){
  Mat m;
  mat.convertTo(m,CV_8U);
  imshow(winname,m);
}

inline void imshowN16(const String &winname,const  Mat &mat){
  Mat m = mat.clone();
  normalize(m,m,0,255*255,NORM_MINMAX);
  imshow(winname,m);
}

inline void imshowN32f(const String &winname,const  Mat &mat){
  Mat m = mat.clone();
  normalize(m,m,0,1,NORM_MINMAX);
  imshow(winname,m);
}

inline float getGray(cv::Vec3b &bgr){
  int B = bgr[0];
  int G = bgr[1];
  int R = bgr[2];
  return 0.299*R+0.587*G+0.114*B;
}

inline cv::Vec3b loadMean(const String &filename, int flags){
  Mat img = imread(filename, flags);
  Scalar m = cv::mean(img);
  return cv::Vec3b(m[0],m[1],m[2]);
}


#define pixel(m,type,x,y) (m).at<type>(y,x)
#define pixelVec3b(m,x,y) (m).at<Vec3b>(y,x)
#define pixel8u(m,x,y) (m).at<uint8_t>(y,x)
#define pixel16u(m,x,y) (m).at<uint16_t>(y,x)
#define pixel32s(m,x,y) (m).at<int32_t>(y,x)
#define pixel32f(m,x,y) (m).at<float>(y,x)
#define pixel64f(m,x,y) (m).at<double>(y,x)
#define pixel8upx(m,px) (m).at<uint8_t>(px)
#define pixel8upxf(m,px) (*(uint8_t*)(m.data+px))
#define pixel16upx(m,px) (m).at<uint16_t>(px)
#define pixel32fpx(m,px) (m).at<float>(px)
#define pixel64fpx(m,px) (m).at<double>(px)
#define pixel8uc3(m,x,y) (m).at<cv::Vec3b>(y,x)
#include "util.h"
inline void pixelInc8U(cv::Mat &m,int x,int y,int inc){
  pixel8u(m,x,y) = constrain(pixel8u(m,x,y)+inc,0,UCHAR_MAX);
}
inline void pixelInc16U(cv::Mat &m,int x,int y,int inc){
  pixel16u(m,x,y) = constrain(pixel16u(m,x,y)+inc,0,USHRT_MAX);
}
// inline void pixelInc8U(cv::Mat &m,int x,int y,int inc){
//   pixel8u(m,x,y) = constrain(pixel8u(m,x,y)+inc,0,UCHAR_MAX);
// }
// inline void pixelInc16U(cv::Mat &m,int x,int y,int inc){
//   pixel16u(m,x,y) = pixel16u(m,x,y)+inc;
// }
// inline void pixelInc32S(cv::Mat &m,int x,int y,int inc){
//   pixel32s(m,x,y) = constrain(pixel32s(m,x,y)+inc,INT_MIN,INT_MAX);
// }
inline void pixelInc32f(cv::Mat &m,int x,int y,float inc){
  pixel32f(m,x,y) = constrain(pixel32f(m,x,y)+inc,-FLT_MAX,FLT_MAX);
}

inline void pixelInc8U(cv::Mat &m,int i,int inc){
  ((uint8_t*)(m.data))[i] = constrain(((uint8_t*)(m.data))[i]+inc,0,UCHAR_MAX);
}
inline void pixelInc16U(cv::Mat &m,int i,int inc){
  ((uint16_t*)(m.data))[i] = constrain(((uint16_t*)(m.data))[i]+inc,0,USHRT_MAX);
}
inline void pixelInc32S(cv::Mat &m,int i,int inc){
  ((int32_t*)(m.data))[i] = constrain(((int32_t*)(m.data))[i]+inc,0,INT_MAX);
}

inline bool pixelInRange(cv::Mat &m, int x, int y){
  return x<m.cols && x>0 && y<m.rows && y>0;
}