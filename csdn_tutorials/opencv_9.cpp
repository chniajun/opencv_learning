//-----------------------------------【程序说明】----------------------------------------------
//            说明：【中值滤波medianBlur函数的使用示例程序】
//            开发所用OpenCV版本：2.4.8
//            2014年4月3 日 Create by 浅墨
//------------------------------------------------------------------------------------------------

//-----------------------------------【头文件包含部分】---------------------------------------
//     描述：包含程序所依赖的头文件
//----------------------------------------------------------------------------------------------
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

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
    Mat image = imread("1.jpg");

    //创建窗口
    namedWindow("中值滤波【原图】");
    namedWindow("中值滤波【效果图】");

    //显示原图
    imshow("中值滤波【原图】", image);

    //进行中值滤波操作
    Mat out;
    medianBlur(image, out, 7);

    //显示效果图
    imshow("中值滤波【效果图】", out);

    waitKey(0);
}