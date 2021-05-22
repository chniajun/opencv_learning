
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include "/usr/include/eigen3/Eigen/Cholesky"
#include "/usr/include/eigen3/Eigen/Core"
#include "/usr/include/eigen3/Eigen/LU"


#define COST_OBS_ROS 200	

#define POT_HIGH 1.0e10	

using namespace cv;
using namespace std;
using namespace Eigen;

vector<int> line_point_x, line_point_y;
void Bresenham_Line(int x1, int y1,
                            int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    /*直线斜率判断*/
    bool k_flag = false;

    if (dx < dy)
    {
        k_flag = true;
        swap(x1, y1);
        swap(x2, y2);
        swap(dx, dy);
    }

    int ix = (x2 - x1) > 0 ? 1 : -1,
        iy = (y2 - y1) > 0 ? 1 : -1,
        cx = x1,
        cy = y1,
        n2dy = dy * 2,
        n2dydx = (dy - dx) * 2,
        d = dy * 2 - dx;

    if (k_flag)
    {
        /*直线与 x 轴的夹角大于 45 度*/
        while (cx != x2)
        {
            if (d < 0)
            {
                d += n2dy;
            }
            else
            {
                cy += iy;
                d += n2dydx;
            }
            line_point_x.push_back(cy);
            line_point_y.push_back(cx);
            cx += ix;
        }
    }
    else
    {
        /*直线与 x 轴的夹角小于 45 度*/
        while (cx != x2)
        {
            if (d < 0)
            {
                d += n2dy;
            }
            else
            {
                cy += iy;
                d += n2dydx;
            }
            line_point_x.push_back(cx);
            line_point_y.push_back(cy);

            cx += ix;
        }
    }
}


double normpdf(double dist, double mu, double sigma)
{
    return exp(-(dist - mu) * (dist - mu) / (2 * sigma * sigma)) / (sqrt(2 * 3.14) * sigma);
}

int main(int argc, char** argv)
{
    Eigen::MatrixXd hit_;     // image coordinate
    Eigen::MatrixXd loadmap_; // gaussian map world coordinate
    Eigen::MatrixXd map_;     // map world coordinate

    Eigen::Matrix3d initconv_;
    Eigen::Matrix3d latestconv_;
    Eigen::MatrixXd GaussianMap_;

    int rows_ = 3 / 0.01;
    int cols_ = 3 / 0.01;  

    double line_length = 0.22;
    double line_angle = 140;
    double resolution_ = 0.01;

    hit_ = MatrixXd::Zero(rows_, cols_); // Col major , not row major
    cv::Mat map_image(rows_, cols_, CV_8U, cv::Scalar(50));

    /*充电站折线 start*/
    double theta = line_angle * M_PI / 180 / 2;
    double line_x = line_length * cos(theta);
    double line_y = line_length * sin(theta);
    int cross_col = cols_ * 5 / 6;
    int cross_row = rows_ / 2;
    int point1_col = cols_ * 5 / 6 - line_x / resolution_;
    int point1_row = rows_ / 2 - line_y / resolution_;
    int point2_col = cols_ * 5 / 6 - line_x / resolution_;
    int point2_row = rows_ / 2 + line_y / resolution_;

    Bresenham_Line(point1_col, point1_row, cross_col, cross_row);
    Bresenham_Line(point2_col, point2_row, cross_col, cross_row);
    line_point_y.push_back(cross_row);
    line_point_x.push_back(cross_col);

    unsigned char *p;
    for (int i = 0; i < line_point_y.size(); i++)
    {
        p = map_image.ptr<uchar>(line_point_y[i]);
        *(p + line_point_x[i]) = 1;
        hit_(line_point_y[i], line_point_x[i]) = 1;
    }
    /*end*/

    imshow("xls",map_image);

    loadmap_ = MatrixXd::Zero(cols_, rows_); // swap width and height
    map_ = MatrixXd::Zero(cols_, rows_);
    MatrixXd tempMap = MatrixXd::Zero(rows_, cols_);
    MatrixXd positivemap = MatrixXd::Zero(rows_, cols_);
    MatrixXd tempPositivemap = MatrixXd::Zero(rows_, cols_);

    //Gaussian filter
    int windowSize = 10;
    double sigma = windowSize * resolution_ / 3;
    MatrixXd gaussianWindow = MatrixXd::Zero(2 * windowSize + 1, 2 * windowSize + 1);
    double max_prob = normpdf(0, 0, sigma);
    for (int i = -windowSize; i < windowSize + 1; i++)
    {
        for (int j = -windowSize; j < windowSize + 1; j++)
        {
            double dist = resolution_ * sqrt(i * i + j * j);
            gaussianWindow(i + windowSize, j + windowSize) = normpdf(dist, 0, sigma) / max_prob;
        }
    }

    for (int y = 0; y < rows_; y++)
    {
        for (int x = 0; x < cols_; x++)
        {
            if (hit_(y, x) == 1)
            {
                for (int i = -windowSize; i < windowSize + 1; i++)
                {
                    for (int j = -windowSize; j < windowSize + 1; j++)
                    {
                        bool inmap = (y + i > -1) && (y + i < rows_) && (x + j > -1) && (x + j < cols_);
                        if (inmap)
                        {
                            double p = positivemap(y + i, x + j);
                            double q = gaussianWindow(i + windowSize, j + windowSize);
                            //positivemap(y + i, x + j) = (p + q) > 1 ? 1 : (p + q); //positivemap is same as Hmap.lookup original
                            if (p < q)
                                positivemap(y + i, x + j) = q;
                        }
                    }
                }
            }
        }
    }

    for (size_t y = 0; y < rows_; ++y)
    {
        for (size_t x = 0; x < cols_; ++x)
        {
            double data = positivemap(y, x);
            tempPositivemap(rows_ - y - 1, x) = data;
            tempMap(rows_ - y - 1, x) = hit_(y, x);
        }
    }

    map_ = tempMap.transpose();

    loadmap_ = tempPositivemap.transpose();

    Mat image(rows_, cols_, CV_8U, Scalar(128));

    unsigned char *p1;

    for (size_t y = 0; y < rows_; ++y)
    {
        p1 = image.ptr<uchar>(y);
        for (size_t x = 0; x < cols_; ++x)
        {
            //const size_t i = x + (rows_ - y - 1) * cols_;
            int data = positivemap(y, x) * 255;
            *(p1 + x) = data;
        }
    }

    imshow("debug",image);
    GaussianMap_ = loadmap_;

    cout<<"GaussianMap_ :" << GaussianMap_<<endl;

    
    waitKey(0);

    

    return 0;

}

