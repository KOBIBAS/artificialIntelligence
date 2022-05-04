#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

// создаем Mat объект, в который будет записываться каждый кадр видео
Mat img;
//создаем вектор с точками, сюда записываются все точки, которые были нарисованы
vector<vector<int>> newPoints;

//цвета в режиме HSV
//эти параметры были получены вручную, с помощью программы по детектированию цветов
vector<vector<int>> myColors{
	//Hue Min, Sat Min, Val Min, Hue Max, Sat Max, Val Max
	{100,88,56,120,239,232}, //blue
	{148,130,156,179,255,255} //pink
};

//вектор с цветами, которыми будем рисовать
vector<Scalar> myColorValues{
	{255,0,0}, //blue
	{255,0,255} //pink
};

//получение контуров по маске
Point getContours(Mat imgDil) {
	//создаем контуры
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//вектор для полигонов
	vector<vector<Point>> conPoly(contours.size());
	//вектор для обводки
	vector<Rect> boundRect(contours.size());

	//точка, от которой будет рисоваться
	Point myPoint(0,0);

	//перебираем все найденные контуры
	for (int i = 0; i < contours.size(); i++) {
		//ищем их площадь
		int area = contourArea(contours[i]);
		//cout << area << endl;

		if (area > 200) { //ограничиваем площадь
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			//drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
			boundRect[i] = boundingRect(conPoly[i]);
			//rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 3);
			//находим точку, от которой будем рисовать
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;
			myPoint.y = boundRect[i].y;
		}
	}
	return myPoint; //возвращаем эту точку
}

vector<vector<int>> findColor(Mat img) {
	Mat imgHSV;
	//переводим в режим HSV
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	//перебираем элементы вектора цветов, которые мы получили при детектировании
	for (int i = 0; i < myColors.size(); i++) {
		//задаем промежуток цветов
		Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
		Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);
		Mat mask;
		//создаем маску
		inRange(imgHSV, lower, upper, mask);
		//imshow(to_string(i), mask);
		//получаем точку, от которой будем рисовать
		Point myPoint = getContours(mask);

		//добавляем точку в в полный вектор точек
		if (myPoint.x != 0 && myPoint.y != 0) {
			newPoints.push_back({ myPoint.x, myPoint.y, i });
		}
	}
	return newPoints; //возвращаем полный вектор точек
}

//рисуем круги на всех точках, через которые прошли контуры
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
		//mode - выбор режима.
		//mode == -1 - не рисуем
		//mode == 1 - рисуем
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
		if (key == 27) { //если нажата ESC - выходим
			break;
		}
		else if (key == 13) { //если нажата Enter - переключаем режим
			mode *= -1;
		}
		else if (key == 8) { //если нажата Backspace - очищаем нарисованный экран
			newPoints.clear();
		}
	}

	return 0;
}