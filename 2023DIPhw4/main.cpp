#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <cmath>

using namespace cv;
using namespace std;

void calcPSF(Mat& outputImg, Size filterSize, int len, double theta);
void fftshift(const Mat& inputImg, Mat& outputImg);
void filter2DFreq(const Mat& inputImg, Mat& outputImg, const Mat& H);
void calcWnrFilter(const Mat& input_h_PSF, Mat& output_G, double nsr);
double calculatePSNR(const Mat& original, const Mat& processed);
void sharpen(const Mat& inputImg, Mat& outputImg);

int main(){

    string InputFileName = "input2.bmp";
    Mat blurImg;
    blurImg = imread(InputFileName, IMREAD_COLOR);
    
    if (blurImg.empty()){
        cout << "ERROR : Image cannot be loaded..!!" << endl;
        return -1;
    }

    // Splitting the image into its component channels
    vector<Mat> channels(3);
    split(blurImg, channels);

    // length of a motion
    int LEN = 25;
    // angle of a motion in degrees
    double THETA = 45;
    // signal to noise ratio
    int snr = 25;

    for(int i = 0; i < 3; ++i){
        // Ensure channel are even image
        Rect roi = Rect(0, 0, channels[i].cols & -2, channels[i].rows & -2);
        //Hw calculation (start)
        Mat Hw, h;
        calcPSF(h, roi.size(), LEN, THETA);
        calcWnrFilter(h, Hw, 1.0 / double(snr));
        //Hw calculation (stop)
        channels[i].convertTo(channels[i], CV_32F);
        // filtering (start)
        filter2DFreq(channels[i](roi), channels[i], Hw);
        // filtering (stop)
        channels[i].convertTo(channels[i], CV_8U);
        normalize(channels[i], channels[i], 0, 255, NORM_MINMAX);
    }

    // Merge processed channels
    Mat imgOut;
    merge(channels, imgOut);
    // Save the output image
    string eval;
    cout << "eval(1 or 0): " << endl;
    cin >> eval;
    imwrite("result_deblur2.bmp", imgOut);

    sharpen(imgOut, imgOut);
    imwrite("result_sharpen2.bmp", imgOut);

    if (eval == "1"){
        double psnr;
        string InputFileName = "input1_ori.bmp";
        Mat oriImg;
        oriImg = imread(InputFileName, IMREAD_COLOR);
        psnr = calculatePSNR(oriImg, blurImg);
        cout << "PSNR: " << psnr << endl;
    }
    
    return 0;
}

void calcPSF(Mat& outputImg, Size filterSize, int len, double theta)
{
    Mat h(filterSize, CV_32F, Scalar(0));
    Point point(filterSize.width / 2, filterSize.height / 2);
    ellipse(h, point, Size(0, cvRound(float(len) / 2.0)), 90.0 - theta, 0, 360, Scalar(255), FILLED);
    Scalar summa = sum(h);
    outputImg = h / summa[0];
}

void fftshift(const Mat& inputImg, Mat& outputImg)
{
    outputImg = inputImg.clone();
    int cx = outputImg.cols / 2;
    int cy = outputImg.rows / 2;
    Mat q0(outputImg, Rect(0, 0, cx, cy));
    Mat q1(outputImg, Rect(cx, 0, cx, cy));
    Mat q2(outputImg, Rect(0, cy, cx, cy));
    Mat q3(outputImg, Rect(cx, cy, cx, cy));
    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

void filter2DFreq(const Mat& inputImg, Mat& outputImg, const Mat& H)
{
    Mat planes[2] = { Mat_<float>(inputImg.clone()), Mat::zeros(inputImg.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI, DFT_SCALE);
    Mat planesH[2] = { Mat_<float>(H.clone()), Mat::zeros(H.size(), CV_32F) };
    Mat complexH;
    merge(planesH, 2, complexH);
    Mat complexIH;
    mulSpectrums(complexI, complexH, complexIH, 0);
    idft(complexIH, complexIH);
    split(complexIH, planes);
    outputImg = planes[0];
}

void calcWnrFilter(const Mat& input_h_PSF, Mat& output_G, double nsr)
{
    Mat h_PSF_shifted;
    fftshift(input_h_PSF, h_PSF_shifted);
    Mat planes[2] = { Mat_<float>(h_PSF_shifted.clone()), Mat::zeros(h_PSF_shifted.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI);
    split(complexI, planes);
    Mat denom;
    pow(abs(planes[0]), 2, denom);
    denom += nsr;
    divide(planes[0], denom, output_G);
}

void sharpen(const Mat& inputImg, Mat& outputImg)
{
    Mat sharpening_kernel = (Mat_<double>(3, 3) << 0, -1, 0,
        -1, 5, -1,
        0, -1, 0);
    filter2D(inputImg, outputImg, -1, sharpening_kernel);    
}

double calculatePSNR(const Mat& I1, const Mat& I2)
{
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2
    Scalar s = sum(s1);        // sum elements per channel
    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels
    if( sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double mse  = sse / (double)(I1.channels() * I1.total());
        double psnr = 10.0 * log10((255 * 255) / mse);
        return psnr;
    }
}