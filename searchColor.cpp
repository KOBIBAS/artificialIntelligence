#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;


Mat imgHSV, mask;
int hmin = 0, smin = 0, vmin = 0;
int hmax = 179, smax = 255, vmax = 255;


///////////////// Color Detection ///////////////////////
int main()
{
	VideoCapture cap(0);
	Mat img;

	//преобразуем BGR to HSV 

	//добавляем трек бар для обнаружения цветов
	namedWindow("Trackbars", (640, 200));

	//
	createTrackbar("Hue Min", "Trackbars", &hmin, 179);
	createTrackbar("Hue Max", "Trackbars", &hmax, 179);

	createTrackbar("Sat Min", "Trackbars", &smin, 255);
	createTrackbar("Sat Max", "Trackbars", &smax, 255);

	createTrackbar("Val Min", "Trackbars", &vmin, 255);
	createTrackbar("Val Max", "Trackbars", &vmax, 255);

	while (true) {
		cap.read(img);
		if (!img.empty())
			cvtColor(img, imgHSV, COLOR_BGR2HSV);

		Scalar lower(hmin, smin, vmin);
		Scalar upper(hmax, smax, vmax);
		if (!imgHSV.empty())
			inRange(imgHSV, lower, upper, mask);

		if (!img.empty())
			imshow("Image", img);
		//imshow("Image HSV", imgHSV);
		if (!mask.empty())
			imshow("Image Mask", mask);
		waitKey(1);
	}


	return 0;
}