//-----------------------------------【头文件包含部分】---------------------------------------
//     描述：包含程序所依赖的头文件
//----------------------------------------------------------------------------------------------
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>

//-----------------------------------【命名空间声明部分】---------------------------------------
//     描述：包含程序所使用的命名空间
//-----------------------------------------------------------------------------------------------
using namespace cv;

//-----------------------------------【main( )函数】--------------------------------------------
//     描述：控制台应用程序的入口函数，我们的程序从这里开始
//-----------------------------------------------------------------------------------------------
int main()
{
    //载入原图
    Mat image = imread("3.png");

    //创建窗口
    namedWindow("高斯滤波【原图】");
    namedWindow("高斯滤波【效果图】");

    //显示原图
    imshow("高斯滤波【原图】", image);

    //进行均值滤波操作
    Mat out;
    GaussianBlur(image, out, Size(3, 3), 0, 0);

    //显示效果图
    imshow("高斯滤波【效果图】", out);

    waitKey(0);
    return 0;
}