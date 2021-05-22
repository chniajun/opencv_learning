//-----------------------------------【程序说明】----------------------------------------------
//  程序名称:：【OpenCV入门教程之四】 创建Trackbar&图像对比度、亮度值调整 配套博文源码
// VS2010版  OpenCV版本：2.4.8
//  2014年3月18 日 Create by 浅墨
//------------------------------------------------------------------------------------------------

//-----------------------------------【头文件包含部分】---------------------------------------
//     描述：包含程序所依赖的头文件
//----------------------------------------------------------------------------------------------
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

//-----------------------------------【命名空间声明部分】---------------------------------------
//     描述：包含程序所使用的命名空间
//-----------------------------------------------------------------------------------------------
using namespace std;
using namespace cv;

//-----------------------------------【全局函数声明部分】--------------------------------------
//     描述：全局函数声明
//-----------------------------------------------------------------------------------------------
static void ContrastAndBright(int, void *);

//-----------------------------------【全局变量声明部分】--------------------------------------
//     描述：全局变量声明
//-----------------------------------------------------------------------------------------------
int g_nContrastValue; //对比度值
int g_nBrightValue;   //亮度值
Mat g_srcImage, g_dstImage;
//-----------------------------------【main( )函数】--------------------------------------------
//     描述：控制台应用程序的入口函数，我们的程序从这里开始
//-----------------------------------------------------------------------------------------------
int main()
{

    //读入用户提供的图像
    g_srcImage = imread("pic1.png");
    if (!g_srcImage.data)
    {
        printf("Oh，no，读取g_srcImage图片错误~！\n");
        return false;
    }
    g_dstImage = Mat::zeros(g_srcImage.size(), g_srcImage.type());// 生成同尺寸的图

    //设定对比度和亮度的初值
    g_nContrastValue = 100;
    g_nBrightValue = 0;

    //创建窗口
    namedWindow("【效果图窗口】", 1);

    //创建轨迹条
    createTrackbar("对比度：", "【效果图窗口】", &g_nContrastValue, 300, ContrastAndBright); // 100 原状
    createTrackbar("亮   度：", "【效果图窗口】", &g_nBrightValue, 255, ContrastAndBright); // 0 原状

    //调用回调函数
    ContrastAndBright(g_nContrastValue, 0);
    ContrastAndBright(g_nBrightValue, 0);

    // //输出一些帮助信息
    // cout << endl
    //      << "\t嗯。好了，请调整滚动条观察图像效果~\n\n"
    //      << "\t按下“q”键时，程序退出~!\n"
    //      << "\n\n\t\t\t\tby浅墨";

    // //按下“q”键时，程序退出
    // while (char(waitKey(1)) != 'q')
    // {
    // }
    waitKey();

    return 0;
}

//-----------------------------【ContrastAndBright( )函数】------------------------------------
//     描述：改变图像对比度和亮度值的回调函数
//-----------------------------------------------------------------------------------------------
static void ContrastAndBright(int, void *)
{

    //创建窗口
    namedWindow("【原始图窗口】", 1);

    //三个for循环，执行运算 g_dstImage(i,j) =a*g_srcImage(i,j) + b
    for (int y = 0; y < g_srcImage.rows; y++)
    {
        for (int x = 0; x < g_srcImage.cols; x++)
        {
            for (int c = 0; c < 3; c++)
            { // y是像素所在的行， x是像素所在的列， c是R、G、B（对应0、1、2）其中之一
                g_dstImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>((g_nContrastValue * 0.01) * (g_srcImage.at<Vec3b>(y, x)[c]) + g_nBrightValue);
            } //  saturate_cast<uchar>主要是为了防止颜色溢出操作 0~255
        }     // 对比度取值为0.0到3.0的浮点值 ， 方便进度条显示，0.01
    }

    //显示图像
    imshow("【原始图窗口】", g_srcImage);
    imshow("【效果图窗口】", g_dstImage);
}
