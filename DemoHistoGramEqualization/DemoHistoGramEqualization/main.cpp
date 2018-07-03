#include <stdio.h>
#include <string>
#include <iostream>
#include <sys/timeb.h>
#include <windows.h>
#include "ImageEnhancer.h"
#include <conio.h>

using namespace std;



void EnhanceImage(cv::Mat& inputImage)
{
	

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
		
		eEqualizationType eqtype = eEqualizationType::HSVSPACE;
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

	TestLive(imageEnhancer);


	system("pause");
	return 0;
}