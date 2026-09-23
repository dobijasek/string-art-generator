#pragma once
#include <opencv2/opencv.hpp>

Scalar getMSSIM( const Mat& i1, const Mat& i2, const Mat& mask, Mat& out)
{
    
    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d = CV_32F;
 
    Mat I1, I2;
    i1.copyTo(I1,mask);
    i2.copyTo(I2,mask);
    I1.convertTo(I1, d);            // cannot calculate on one byte large values
    I2.convertTo(I2, d);
    Mat I2_2   = I2.mul(I2);        // I2^2
    Mat I1_2   = I1.mul(I1);        // I1^2
    Mat I1_I2  = I1.mul(I2);        // I1 * I2
 
    /*************************** END INITS **********************************/
 
    Mat mu1, mu2;                   // PRELIMINARY COMPUTING
    int r = random(3,19);
    r = r%2==0 ? r+1 : r;
    r = 11;
    GaussianBlur(I1, mu1, Size(r, r), 1.5);
    GaussianBlur(I2, mu2, Size(r, r), 1.5);
 
    Mat mu1_2   =   mu1.mul(mu1);
    Mat mu2_2   =   mu2.mul(mu2);
    Mat mu1_mu2 =   mu1.mul(mu2);
 
    Mat sigma1_2, sigma2_2, sigma12;
    GaussianBlur(I1_2, sigma1_2, Size(r, r), 1.5);
    sigma1_2 -= mu1_2;
 
    GaussianBlur(I2_2, sigma2_2, Size(r, r), 1.5);
    sigma2_2 -= mu2_2;
 
    GaussianBlur(I1_I2, sigma12, Size(r, r), 1.5);
    sigma12 -= mu1_mu2;
 
    Mat t1, t2, t3;
 
    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);                 // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
 
    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);                 // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
 
    Mat ssim_map;
    divide(t3, t1, ssim_map);        // ssim_map =  t3./t1;
    out = 1-ssim_map.clone();
    imshow("map",out);
    // ssim_map.convertTo(out,CV_64F);
    Scalar mssim = mean(ssim_map);   // mssim = average of ssim map
    return mssim;
}