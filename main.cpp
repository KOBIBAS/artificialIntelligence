#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

// ������� Mat ������, � ������� ����� ������������ ������ ���� �����
Mat img;
//������� ������ � �������, ���� ������������ ��� �����, ������� ���� ����������
vector<vector<int>> newPoints;

//����� � ������ HSV
//��� ��������� ���� �������� �������, � ������� ��������� �� �������������� ������
vector<vector<int>> myColors{
	//Hue Min, Sat Min, Val Min, Hue Max, Sat Max, Val Max
	{100,88,56,120,239,232}, //blue
	{148,130,156,179,255,255} //pink
};

//������ � �������, �������� ����� ��������
vector<Scalar> myColorValues{
	{255,0,0}, //blue
	{255,0,255} //pink
};

//��������� �������� �� �����
Point getContours(Mat imgDil) {
	//������� �������
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//������ ��� ���������
	vector<vector<Point>> conPoly(contours.size());
	//������ ��� �������
	vector<Rect> boundRect(contours.size());

	//�����, �� ������� ����� ����������
	Point myPoint(0,0);

	//���������� ��� ��������� �������
	for (int i = 0; i < contours.size(); i++) {
		//���� �� �������
		int area = contourArea(contours[i]);
		//cout << area << endl;

		if (area > 200) { //������������ �������
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			//drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
			boundRect[i] = boundingRect(conPoly[i]);
			//rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 3);
			//������� �����, �� ������� ����� ��������
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;
			myPoint.y = boundRect[i].y;
		}
	}
	return myPoint; //���������� ��� �����
}

vector<vector<int>> findColor(Mat img) {
	Mat imgHSV;
	//��������� � ����� HSV
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	//���������� �������� ������� ������, ������� �� �������� ��� ��������������
	for (int i = 0; i < myColors.size(); i++) {
		//������ ���������� ������
		Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
		Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);
		Mat mask;
		//������� �����
		inRange(imgHSV, lower, upper, mask);
		//imshow(to_string(i), mask);
		//�������� �����, �� ������� ����� ��������
		Point myPoint = getContours(mask);

		//��������� ����� � � ������ ������ �����
		if (myPoint.x != 0 && myPoint.y != 0) {
			newPoints.push_back({ myPoint.x, myPoint.y, i });
		}
	}
	return newPoints; //���������� ������ ������ �����
}

//������ ����� �� ���� ������, ����� ������� ������ �������
void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues) {
	for (int i = 0; i < newPoints.size(); i++) {
		circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, myColorValues[newPoints[i][2]], FILLED);
	}
}


int main()
{
	VideoCapture cap(0);
	int key, mode = -1;
	Mat myImg;
	while (true)
	{
		cap.read(myImg);
		flip(myImg, img, 1);
		//mode - ����� ������.
		//mode == -1 - �� ������
		//mode == 1 - ������
		if (mode == -1) { 
			putText(img, "Drawing - OFF", { 10, 10 }, FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 255), 1);
		}
		else if (mode == 1) {
			putText(img, "Drawing - ON", { 10, 10 }, FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 255, 0), 1);
			newPoints = findColor(img);
		}
			drawOnCanvas(newPoints, myColorValues);
			imshow("Camera", img);
		key = waitKey(1);
		if (key == 27) { //���� ������ ESC - �������
			break;
		}
		else if (key == 13) { //���� ������ Enter - ����������� �����
			mode *= -1;
		}
		else if (key == 8) { //���� ������ Backspace - ������� ������������ �����
			newPoints.clear();
		}
	}

	return 0;
}