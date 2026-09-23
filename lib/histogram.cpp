#include "histogram.h"
#include "core.h"

using namespace cv;
extern core_t srcc;
cv::Mat getHistogram(const hists_t &mats){
    int histSize = 256;
    float range[] = { 0, 256 }; //the upper boundary is exclusive
    const float* histRange[] = { range };
    bool uniform = true, accumulate = false;
    Mat mix_histogram,dst_histogram,src_histogram;
    calcHist( &mats.mix, 1, 0, srcc.mask, mix_histogram, 1, &histSize, histRange, uniform, accumulate );
    calcHist( &mats.dst, 1, 0, srcc.mask, dst_histogram, 1, &histSize, histRange, uniform, accumulate );
    calcHist( &mats.src, 1, 0, srcc.mask, src_histogram, 1, &histSize, histRange, uniform, accumulate );
    int wScale = 4;
    int hist_w = 256*wScale, hist_h = 800;
    int bin_w = cvRound( (double) hist_w/histSize );
    Mat histogram_img( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
    double minMix, maxMix, minDst, maxDst, minSrc, maxSrc;
    minMaxLoc(mix_histogram, &minMix, &maxMix);
    minMaxLoc(dst_histogram, &minDst, &maxDst);
    minMaxLoc(src_histogram, &minSrc, &maxSrc);

    double globalMin = std::min({minMix, minDst, minSrc});
    double globalMax = std::max({maxMix, maxDst, maxSrc});

    double scale = histogram_img.rows / (globalMax - globalMin);
    // Ręczna normalizacja każdego histogramu
    for (int i = 0; i < histSize; i++) {
        mix_histogram.at<float>(i) = (mix_histogram.at<float>(i) - globalMin) * scale;
        dst_histogram.at<float>(i) = (dst_histogram.at<float>(i) - globalMin) * scale;
        src_histogram.at<float>(i) = (src_histogram.at<float>(i) - globalMin) * scale;
    }
    auto colors = srcc.getColors();
    for(int i=0;i<4;i++){
        int x = colors.at(i)*wScale;
        int y = hist_h-1;
        auto p1 = Point(x,0);
        auto p2 = Point(x,y);
        line( histogram_img, p1, p2,Scalar( 255, 0, 0), 1, LINE_AA, 0  );
    }
    for( int i = 1; i < histSize; i++ )
    {
        line( histogram_img, Point( bin_w*(i-1), hist_h - cvRound(src_histogram.at<float>(i-1)) ),
                Point( bin_w*(i), hist_h - cvRound(src_histogram.at<float>(i)) ),
                Scalar( 200, 32, 111), 1, LINE_AA, 0  );
        line( histogram_img, Point( bin_w*(i-1), hist_h - cvRound(mix_histogram.at<float>(i-1)) ),
                Point( bin_w*(i), hist_h - cvRound(mix_histogram.at<float>(i)) ),
                Scalar( 0, 255, 0), 1, LINE_AA, 0  );
        line( histogram_img, Point( bin_w*(i-1), hist_h - cvRound(dst_histogram.at<float>(i-1)) ),
                Point( bin_w*(i), hist_h - cvRound(dst_histogram.at<float>(i)) ),
                Scalar( 0, 0, 255), 1, LINE_AA, 0  );
    }
    return histogram_img;
}