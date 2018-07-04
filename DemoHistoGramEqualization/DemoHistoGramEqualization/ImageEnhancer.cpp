#include "ImageEnhancer.h"



CImageEnhancer::CImageEnhancer()
{
}


CImageEnhancer::~CImageEnhancer()
{
}
void CImageEnhancer::correctGamma(Mat& img, Mat& out, double gamma) {
	double inverse_gamma = 1.0 / gamma;

	Mat lut_matrix(1, 256, CV_8UC1);
	uchar * ptr = lut_matrix.ptr();
	for (int i = 0; i < 256; i++)
		ptr[i] = (int)(pow((double)i / 255.0, inverse_gamma) * 255.0);
		
	LUT(img, lut_matrix, out);
	
}

void CImageEnhancer::EqualizeHistogram(Mat & img, Mat & out, bool fUseAdaptive)
{
	if (img.channels() == 1 && !img.empty())
	{
		if (fUseAdaptive)
		{
			int iKernelSize = 8;
			double dClipLimit = 2.0;
			cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
			clahe->setClipLimit(dClipLimit);
			clahe->setTilesGridSize(cv::Size(iKernelSize, iKernelSize));
			cv::Mat dst;
			clahe->apply(img, out);
		}
		else
			equalizeHist(img, out);
	}
}

bool CImageEnhancer::EqualizeHistogram(cv::Mat inputImage, cv::Mat & outputImage, eEqualizationType _equalizationtype, bool fuseAdptive, bool fUseGamma, double dGamma)
{
	assert(inputImage.empty() != true);
	Mat result;

	if (inputImage.channels() == 1)
	{
		equalizeHist(inputImage, result);
	}
	else if (inputImage.channels() == 3)
	{
		vector<Mat> channels;
		switch (_equalizationtype)
		{

		case eEqualizationType::HSVSPACE:
			{
				Mat hsv;
				cvtColor(inputImage, hsv, CV_BGR2HSV);
				split(hsv, channels);
				EqualizeHistogram(channels[2], channels[2],fuseAdptive);
				if (fUseGamma)
					correctGamma(channels[2], channels[2], dGamma);
				merge(channels, hsv);
				cvtColor(hsv, result, CV_HSV2BGR);
			}
			break;
		case eEqualizationType::YCRCBSPACE:
			{
				Mat ycrcb;
				cvtColor(inputImage, ycrcb, CV_BGR2YCrCb);
				split(ycrcb, channels);
				EqualizeHistogram(channels[0], channels[0], fuseAdptive);
				if (fUseGamma)
					correctGamma(channels[0], channels[0], dGamma);
				merge(channels, ycrcb);
				cvtColor(ycrcb, result, CV_YCrCb2BGR);
			}
			break;
		case eEqualizationType::LABSPACE:
			{
				Mat lab;
				cvtColor(inputImage, lab, CV_BGR2Lab);
				split(lab, channels);
				EqualizeHistogram(channels[0], channels[0], fuseAdptive);
				if (fUseGamma)
					correctGamma(channels[0], channels[0], dGamma);
				merge(channels, lab);
				cvtColor(lab, result, CV_Lab2BGR);
			}
			break;		
		default:
			break;
		}

		


		
	}
	if (!result.empty())
		result.copyTo(outputImage);

	return !result.empty();
}

void CImageEnhancer::GetColorHistogram(cv::Mat inputImage, cv::Mat & histogramImage)
{
	if (inputImage.channels() != 3)
		return;

	cv::Mat gray;
	cv::cvtColor(inputImage, gray, cv::COLOR_RGB2GRAY);

	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split(inputImage, bgr_planes);

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat b_hist, g_hist, r_hist, value_hist;

	/// Compute the histograms:
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&gray, 1, 0, Mat(), value_hist, 1, &histSize, &histRange, uniform, accumulate);

	// Draw the histograms for B, G and R
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(value_hist, value_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);

		/*line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(value_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(value_hist.at<float>(i))),
			Scalar(255, 255, 255), 2, 8, 0);*/
	}

	histImage.copyTo(histogramImage);

	return;
}
