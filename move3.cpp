#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
using namespace cv;
using namespace std;

int n = 5;
int center = (n - 1) / 2;
double sigma = 1.4;

double gauss(int x, int y, int a, int b) {
    return 
    1.0 / (2 * M_PI * sigma * sigma) * 
    exp(-((x - a) * (x - a) + (y - b) * (y - b)) / 
    (2 * sigma * sigma));
}

vector<vector<double>> getMatrix() {
    vector<vector<double>> _matrix = vector<vector<double>>();
    for (int i = 0; i < n; i++) {
        vector<double> _vector = vector<double>();
        for (int j = 0; j < n; j++) {
            _vector.push_back(gauss(i, j, center, center));
        }
        _matrix.push_back(_vector);
    }
    return _matrix;
}

double getSum(vector<vector<double>> &matrix) {
    double _sum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            _sum += matrix[i][j];
        }
    }
    return _sum;
}

vector<vector<double>> getNormMatrix(vector<vector<double>> &matrix, double sum) {
    vector<vector<double>> _matrix = vector<vector<double>>();
    for (int i = 0; i < n; i++) {
        vector<double> _vector = vector<double>();
        for (int j = 0; j < n; j++) {
            _vector.push_back(matrix[i][j] / sum);
        }
        _matrix.push_back(_vector);
    }
    return _matrix;
}

Mat getBlurPicture(vector<vector<double>> &matrix, Mat &img) {
    Mat imgNew = img.clone();
    int len1 = img.rows;
    int len2 = img.cols;
    
    for (int i = center; i < len1 - center; i++) {
        for (int j = center; j < len2 - center; j++) {
            int sum1 = 0, sum2 = 0, sum3 = 0;
            for (int q = 0; q < n; q++) {
                for (int w = 0; w < n; w++) {
	                auto _img = img.at<Vec3b>(q + i - center, w + j - center);
	                sum1 += matrix[q][w] * _img[0];
	                sum2 += matrix[q][w] * _img[1];
	                sum3 += matrix[q][w] * _img[2];
                }
            }
            imgNew.at<Vec3b>(i, j)[0] = sum1;
            imgNew.at<Vec3b>(i, j)[1] = sum2;
            imgNew.at<Vec3b>(i, j)[2] = sum3;
        }
    }
    return imgNew;
}

auto some_x(Mat img, int n, int m) {
	vector<vector<int>> gx = {	{-1, 0, 1},
							 	{-2, 0, 2},
							 	{-1, 0, 1}};
	Mat imgNew = Mat::zeros(n, m, CV_32F);
	
	for (int i = 1; i < n - 1; i++) {
		for (int j = 1; j < m - 1; j++) {
			auto subImg = img(Range(i - 1, i + 2), Range(j - 1, j + 2));
			int sum1 = 0, sum2 = 0, sum3 = 0;
			Mat obj = Mat::zeros(3, 3, CV_32F);
			for (int f = 0; f < 3; f++)
				for (int g = 0; g < 3; g++) {
					obj.at<char>(f, g) = subImg.at<char>(f, g) * gx[f][g];
					sum1 += obj.at<char>(f, g);
					//sum2 += obj.at<char>(f, g)[1];
					//sum3 += obj.at<char>(f, g)[2];
				}
			imgNew.at<char>(i, j) = sum1;
			//imgNew.at<char>(i, j)[1] = sum2;
			//imgNew.at<char>(i, j)[2] = sum3;
			
		}
	}
	return imgNew;
}

auto some_y(Mat img, int n, int m) {
	vector<vector<int>> gy = {	{-1, -2, -1},
							 	{0, 0, 0},
							 	{1, 2, 1}};
	Mat imgNew = Mat::zeros(n, m, CV_32F);
	
	for (int i = 1; i < n - 1; i++) {
		for (int j = 1; j < m - 1; j++) {
			auto subImg = img(Range(i - 1, i + 2), Range(j - 1, j + 2));
			int sum1 = 0, sum2 = 0, sum3 = 0;
			Mat obj = Mat::zeros(3, 3, CV_32F);
			for (int f = 0; f < 3; f++)
				for (int g = 0; g < 3; g++) {
					obj.at<char>(f, g) = subImg.at<char>(f, g) * gy[f][g];
					sum1 += obj.at<char>(f, g);
					//sum2 += obj.at<char>(f, g)[1];
					//sum3 += obj.at<char>(f, g)[2];
				}
			imgNew.at<char>(i, j) = sum1;
			//imgNew.at<char>(i, j)[1] = sum2;
			//imgNew.at<char>(i, j)[2] = sum3;
			
		}
	}
	return imgNew;
}

void gradient(Mat img, int n, int m) {
	auto gx = some_x(img, n, m);
	auto gy = some_y(img, n, m);
	
	vector<vector<double>> matrix_length = vector<vector<double>>(n, vector<double>(m, 0));
	vector<vector<int>> matrix_atan = vector<vector<int>>(n, vector<int>(m, 0));
	
	//Mat matrix_length = Mat::zeros(n, m, CV_32F);
	//Mat matrix_atan = Mat::zeros(n, m, CV_32F);
	
	for (int i = 1; i < n - 1; i++) {
		for (int j = 1; j < m - 1; j++) {
			auto x = (int)gx.at<char>(i, j);
			auto y = (int)gy.at<char>(i, j);
			matrix_length[i][j] = sqrt(x * x + y * y);
			
			double tg = -1;
			if (x != 0)
				tg = y / (double)x;
			int value = -1;
			
			if (	 x > 0 && y < 0 && tg < -2.414 ||
					 x < 0 && y < 0 && tg > 2.414)
				value = 0;
			else if (x > 0 && y < 0 && tg < -0.414)
				value = 1;
			else if (x > 0 && y < 0 && tg > -0.414 ||
					 x > 0 && y > 0 && tg < 0.414)
				value = 2;
			else if (x > 0 && y > 0 && tg < 2.414)
				value = 3;
			else if (x > 0 && y > 0 && tg > 2.414 ||
					 x < 0 && y > 0 && tg < -2.414)
				value = 4;
			else if (x < 0 && y > 0 && tg < -0.414)
				value = 5;
			else if (x < 0 && y > 0 && tg > -0.414 ||
					 x < 0 && y < 0 && tg < 0.414)
				value = 6;
			else if (x < 0 && y < 0 && tg < 2.414)
				value = 7;

			matrix_atan[i][j] = value;
		}
	}
	
	int low_level = 20;
	int high_level = 60;
	
	Mat matrix_border = img.clone();
	for (int i = 1; i < n - 1; i++) {
		for (int j = 1; j < m - 1; j++) {
			vector<vector<int>> way_plus = {{-1, -1}, {-1, -1}};
			int some = matrix_atan[i][j];
			// [y, x] logic
			if (some == 0 || some == 4)
				way_plus = {{-1, 0}, {1, 0}};
			else if (some == 2 || some == 6)
				way_plus = {{0, -1}, {0, 1}};
			else if (some == 1 || some == 5)
				way_plus = {{-1, 1}, {1, -1}};
			else if (some == 3 || some == 7)
				way_plus = {{1, 1}, {-1, -1}};

			double grad = matrix_length[i][j];

			if (grad >= matrix_length[i + way_plus[0][0]][j + way_plus[0][1]]
			&& grad >= matrix_length[i + way_plus[1][0]][j + way_plus[1][1]])
				matrix_border.at<char>(i, j) = 0;
			else
				matrix_border.at<char>(i, j) = 255;

			if (matrix_border.at<char>(i, j) == 0) {
				matrix_border.at<char>(i, j) = 255;
				auto subImg = matrix_border(Range(i - 1, i + 2), Range(j - 1, j + 2));
				int min_el = 256;
				for (int f = 0; f < 3; f++)
					for (int g = 0; g < 3; g++)
						if (subImg.at<char>(f, g) < min_el)
							min_el = subImg.at<char>(f, g);
				if (grad < low_level)
					matrix_border.at<char>(i, j) = 255;
				else if (grad > high_level)
					matrix_border.at<char>(i, j) = 0;
				else if (min_el == 0)
					matrix_border.at<char>(i, j) = 0;
			}
		}
	}
	
	imshow("name", matrix_border);
	waitKey(0);
}

int main(int argc, char** argv )
{
    Mat image = imread( "./fruit.jpg" );
    image.data ? 0 : -1;
    cvtColor(image, image, COLOR_BGR2GRAY);
    int n = image.rows;
    int m = image.cols;
    
    vector<vector<double>> matrix = getMatrix();
    matrix = getNormMatrix(matrix, getSum(matrix));
    image = getBlurPicture(matrix, image);
    //for (int i = 0; i < n; i++)
    //	for (int j = 0; j < m; j++)
    //		cout << (int)image.at<char>(i, j) << endl;
    gradient(image, n, m);
    
    //imshow("run", image);
    //waitKey(0);
    return 0;
}
