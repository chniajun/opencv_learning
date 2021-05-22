//-----------------------------------【头文件包含部分】---------------------------------------
//     描述：包含程序所依赖的头文件
//----------------------------------------------------------------------------------------------
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

//-----------------------------------【命名空间声明部分】---------------------------------------
//     描述：包含程序所使用的命名空间
//-----------------------------------------------------------------------------------------------
using namespace std;
using namespace cv;

//-----------------------------------【main( )函数】--------------------------------------------
//     描述：控制台应用程序的入口函数，我们的程序从这里开始
//-----------------------------------------------------------------------------------------------
int main()
{

    //载入原图
    Mat image = imread("6.png");

    //创建窗口
    namedWindow("【原图】膨胀操作");
    namedWindow("【效果图】膨胀操作");

    //显示原图
    imshow("【原图】膨胀操作", image);

    //获取自定义核
    Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
    Mat out;
    //进行膨胀操作
    dilate(image, out, element);

    //显示效果图
    imshow("【效果图】膨胀操作", out);

    waitKey(0);

    return 0;
}