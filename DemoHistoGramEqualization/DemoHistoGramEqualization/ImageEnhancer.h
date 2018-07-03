#pragma once

#include <opencv/cv.h>
#include <opencv2\opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

enum class eEqualizationType{ GRAY1CHANNEL = 1, HSVSPACE = 2, YCRCBSPACE = 3, LABSPACE = 4 };


class CImageEnhancer
{
protected:
	void correctGamma(Mat & img, Mat & out, double gamma);
	void EqualizeHistogram(Mat & img, Mat & out, bool fUseAdaptive);
public:
	CImageEnhancer();
	virtual ~CImageEnhancer();
	
public:
	bool EqualizeHistogram(cv::Mat inputImage, cv::Mat& outputImage, eEqualizationType _equalizationtype, bool fuseAdptive = false, bool fUseGamma = false, double dGamma = 1.5);
	void GetColorHistogram(cv::Mat inputImage, cv::Mat& histogramImage);
};

