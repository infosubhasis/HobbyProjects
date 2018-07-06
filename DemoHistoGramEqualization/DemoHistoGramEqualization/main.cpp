#include <stdio.h>
#include <string>
#include <iostream>
#include <sys/timeb.h>
#include <windows.h>
#include "ImageEnhancer.h"
#include <conio.h>

using namespace std;
cv::Mat input;
const int slider_max = 100;
int slider_value;


void on_trackbar(int, void*)
{
	cv::Mat ycrcb, brightImg;
	vector<Mat> channels;

	cvtColor(input, ycrcb, CV_BGR2YCrCb);
	split(ycrcb, channels);

	channels[0] = channels[0] + slider_value;
	merge(channels, ycrcb);
	cvtColor(ycrcb, brightImg, CV_YCrCb2BGR);

	imshow("Brightness added", brightImg);
}

void TestImage(CImageEnhancer & imageEnhancer)
{
	cv::Mat img = cv::imread("demo.png");

	if (!img.empty())
	{
		img.copyTo(input);

		cv::imshow("input", img);		
		
		cv::Mat enhancedImg, enhancedImgAdaptive;

		//imageEnhancer.EqualizeHistogram(img, enhancedImgAdaptive, eEqualizationType::HSVSPACE, true, false, 1.0);
		imageEnhancer.EqualizeHistogram(img, enhancedImg, eEqualizationType::HSVSPACE, false, true, 0.7);



		/// Create Windows
		namedWindow("Brightness added", 1);

		/// Create Trackbars
		char TrackbarName[50] = "Brightness";
		
		slider_value = 0;
		createTrackbar(TrackbarName, "Brightness added", &slider_value, slider_max, on_trackbar);

		/// Show some stuff
		on_trackbar(slider_value, 0);
		cv::waitKey(0);

		if (!enhancedImg.empty())
			cv::imshow("Enhanced", enhancedImg);

		if (!enhancedImgAdaptive.empty())
			cv::imshow("EnhancedAdaptive", enhancedImgAdaptive);
		cv::Mat histImageOriginal, histImageEnhanced;
		
		imageEnhancer.GetColorHistogram(img, histImageOriginal);
		imageEnhancer.GetColorHistogram(enhancedImg, histImageEnhanced);
		if (!histImageOriginal.empty())
			cv::imshow("Original Image : Histogram", histImageOriginal);

		if (!histImageEnhanced.empty())
			cv::imshow("Enahnced Image : Histogram", histImageEnhanced);
		//cv::imwrite("out.png", enhancedImg);
		cv::waitKey(0);
		cv::destroyWindow("input");
		cv::destroyWindow("Enhanced");
		cv::destroyWindow("Brightness added");
	}

}

void TestLive(CImageEnhancer & imageEnhancer)
{
	VideoCapture capture(1);
	Mat frame;
	char key;
	cv::Mat enhancedImageGray, enhancedImageColor, enhancedImageColorAdaptive;
	bool exit = false;

	//std::cout << "press esc to exit! " << std::endl;
	int iOption = 2;  //0 for HSV , 1 for YCrCb, 2 for LAB
	double dGammaCorrectionValue = 1.0;
	double dGammaCorrectionValueAdaptive = 1.0;
	while (exit == false)
	{
		/*if (GetAsyncKeyState(VK_ESCAPE))
		{
			exit = true;
		}*/

		
		
		int64 start = cv::getTickCount();
		capture >> frame;
		if (frame.empty())
			break;
		/*cv::Mat gray;
		cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);*/

		cv::imshow("Original", frame);
		//cv::imshow("OriginalGray", gray);

		/*imageEnhancer.EqualizeHistogram(gray, enhancedImageGray, eEqualizationType::GRAY1CHANNEL);
		if(!enhancedImageGray.empty())
		cv::imshow("Enhanced Image Gray", enhancedImageGray);*/

		cv::Mat histImageOriginal, histImageEnhanced, histImageEnhancedAdapted;		
		
		eEqualizationType eqtype = eEqualizationType::LABSPACE;
		std::string colorspaceName = "";
		
		switch (iOption)
		{
		case 0:
			colorspaceName = "HSV";
			eqtype = eEqualizationType::HSVSPACE;
			break;
		case 1:
			colorspaceName = "YcrCb";
			eqtype = eEqualizationType::YCRCBSPACE;
			break;
		case 2:
			colorspaceName = "Lab";
			eqtype = eEqualizationType::LABSPACE;
			break;
		default:
			break;
		}

		if (_kbhit())
		{
			std::string inputstring;
			cout << "Do You want to Exit ? /\n";
			cout << "y - To exit live feed: \n";
			cout << "n - To continue with Live feed: \n";
			cin >> inputstring;
			if (inputstring.compare("y") != 0)
			{
				if (!colorspaceName.empty())
				{
					destroyWindow("Enhanced Image Color space : " + colorspaceName);
					destroyWindow("Enhanced Image  Adaptive Color space: " + colorspaceName);
				}
				std::cout << "Please enter the colorspace ( 0 : HSV, 1: YCrCb, : LAB )  and gamma values( between 0.1 - 5.0) :  ";
				std::cin >> iOption >> dGammaCorrectionValue >> dGammaCorrectionValueAdaptive;
				continue;
			}
			else
			{
				std::cout << std::endl << "Exiting Live feed..." << std::endl;
				break;
			}

		}
		if (iOption < 0 || iOption > 2 || dGammaCorrectionValue <= 0 || dGammaCorrectionValue > 5 || dGammaCorrectionValueAdaptive <= 0 || dGammaCorrectionValueAdaptive > 5)
		{
			std::cout << std::endl << "Please enter correct colorspace ( 0 : HSV, 1: YCrCb, 2: LAB )  and gamma values :  ";
			std::cin >> iOption >> dGammaCorrectionValue;
		}

		if (colorspaceName.empty())
		{
			std::cout << std::endl << "Error : Invalid Color space. Please Enter corect option ";
			continue;
		}


		imageEnhancer.EqualizeHistogram(frame, enhancedImageColor, eqtype,false,true,dGammaCorrectionValue);		

		if (!enhancedImageColor.empty())
		{
			std::string windowtitle = "Enhanced Image Color space : " + colorspaceName;
			cv::imshow(windowtitle, enhancedImageColor);
			
			imageEnhancer.GetColorHistogram(frame, histImageOriginal);
			imageEnhancer.GetColorHistogram(enhancedImageColor, histImageEnhanced);
		}
		
		imageEnhancer.EqualizeHistogram(frame, enhancedImageColorAdaptive, eqtype, true, true, dGammaCorrectionValueAdaptive);
		if (!enhancedImageColorAdaptive.empty())
		{
			std::string windowtitle = "Enhanced Image  Adaptive Color space: " + colorspaceName;
			cv::imshow(windowtitle, enhancedImageColorAdaptive);
			
			imageEnhancer.GetColorHistogram(enhancedImageColorAdaptive, histImageEnhancedAdapted);
		}
		
		if (!histImageOriginal.empty())
			cv::imshow("Original Image : Histogram", histImageOriginal);

		if (!histImageEnhanced.empty())
			cv::imshow("Enahnced Image : Histogram", histImageEnhanced);

		if (!histImageEnhancedAdapted.empty())
			cv::imshow("Enahnced Image Adaptive: Histogram", histImageEnhancedAdapted);

		cv::waitKey(2);
		double fps = cv::getTickFrequency() / (cv::getTickCount() - start);

		std::cout << "FPS : " << fps << std::endl;
				
	}
}


int main(int argc, char* argv[])
{

	CImageEnhancer imageEnhancer;
	TestImage(imageEnhancer);

	TestLive(imageEnhancer);


	system("pause");
	return 0;
}