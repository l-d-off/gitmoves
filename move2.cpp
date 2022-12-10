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
        //_vector.~vector();
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
        //_vector.~vector();
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
	                //int _img = img.at<int>(q + i - center, w + j - center);
	                //cout << _img << endl;
	                auto _img = img.at<Vec3b>(q + i - center, w + j - center);
	                sum1 += matrix[q][w] * _img[0];
	                sum2 += matrix[q][w] * _img[1];
	                sum3 += matrix[q][w] * _img[2];
                }
            }
            imgNew.at<Vec3b>(i, j)[0] = sum1;
            imgNew.at<Vec3b>(i, j)[1] = sum2;
            imgNew.at<Vec3b>(i, j)[2] = sum3;
            //cout << imgNew.at<int>(i, j) - img.at<int>(i, j) << endl;
        }
    }
    return imgNew;
}

int main(int argc, char** argv )
{
    /*if ( argc != 2 )
    {
        printf("usage: run.out <Image_Path>\n");
        return -1;
    }*/
    Mat image2;
    Mat image;
    image2 = imread( "./cat.jpg" );
    if ( !image2.data )
    {
        printf("No image data \n");
        return -1;
    }
    //cout << "slfd";
    cvtColor(image2, image, COLOR_BGR2GRAY);
    
    vector<vector<double>> matrix = getMatrix();
    double sum = getSum(matrix);
    vector<vector<double>> matrix1 = getNormMatrix(matrix, sum);
    /*for (int i = 0; i < matrix.size(); i++)
        matrix[i].~vector();
    //cout << matrix.size();
    matrix.~vector();*/
    Mat image1 = getBlurPicture(matrix1, image);
    /*for (int i = 0; i < matrix1.size(); i++)
        matrix1[i].~vector();
    matrix1.~vector();
    image2.deallocate();
    image.deallocate();*/
    
    namedWindow("run", WINDOW_NORMAL );
    imshow("run", image1);
    //image1.deallocate();
    waitKey(0);
    return 0;
}
