//-----------------------------------【头文件包含部分】---------------------------------------
//            描述：包含程序所依赖的头文件
//----------------------------------------------------------------------------------------------
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//-----------------------------------【命名空间声明部分】---------------------------------------
//            描述：包含程序所使用的命名空间
//-----------------------------------------------------------------------------------------------
using namespace cv;

//-----------------------------------【main( )函数】--------------------------------------------
//            描述：控制台应用程序的入口函数，我们的程序从这里开始
//-----------------------------------------------------------------------------------------------
int main()
{
    //【0】变量的定义
    Mat src, src_gray, dst, abs_dst;

    //【1】载入原始图
    src = imread("16.png"); //工程目录下应该有一张名为1.jpg的素材图

    //【2】显示原始图
    imshow("【原始图】图像Laplace变换", src);

    //【3】使用高斯滤波消除噪声
    GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);

    //【4】转换为灰度图
    cvtColor(src, src_gray, CV_RGB2GRAY);

    //【5】使用Laplace函数
    Laplacian(src_gray, dst, CV_16S, 3, 1, 0, BORDER_DEFAULT);

    //【6】计算绝对值，并将结果转换成8位
    convertScaleAbs(dst, abs_dst);

    //【7】显示效果图
    imshow("【效果图】图像Laplace变换", abs_dst);

    waitKey(0);

    return 0;
}