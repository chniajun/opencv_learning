//-----------------------------------【程序说明】----------------------------------------------
//		程序名称:：《【OpenCV入门教程之十七】OpenCV重映射 & SURF特征点检测合辑 》 博文配套源码
//		开发所用IDE版本：Visual Studio 2010
//   	开发所用OpenCV版本：	2.4.9
//		2014年5月26日 Created by 浅墨
//		配套博文链接： http://blog.csdn.net/poem_qianmo/article/details/26977557
//		PS:程序结合配合博文学习效果更佳
//		浅墨的微博：@浅墨_毛星云 http://weibo.com/1723155442
//		浅墨的知乎：http://www.zhihu.com/people/mao-xing-yun
//		浅墨的豆瓣：http://www.douban.com/people/53426472/
//----------------------------------------------------------------------------------------------

//-----------------------------------【头文件包含部分】---------------------------------------
//		描述：包含程序所依赖的头文件
//----------------------------------------------------------------------------------------------
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

//-----------------------------------【命名空间声明部分】--------------------------------------
//          描述：包含程序所使用的命名空间
//-----------------------------------------------------------------------------------------------
using namespace cv;

//-----------------------------------【main( )函数】--------------------------------------------
//          描述：控制台应用程序的入口函数，我们的程序从这里开始执行
//-----------------------------------------------------------------------------------------------
int main()
{
    //【0】变量定义
    Mat srcImage, dstImage;
    Mat map_x, map_y;

    //【1】载入原始图
    srcImage = imread("29.png", 1);
    if (!srcImage.data)
    {
        printf("读取图片错误，请确定目录下是否有imread函数指定的图片存在~！ \n");
        return false;
    }
    imshow("原始图", srcImage);

    //【2】创建和原始图一样的效果图，x重映射图，y重映射图
    dstImage.create(srcImage.size(), srcImage.type());
    map_x.create(srcImage.size(), CV_32FC1);
    map_y.create(srcImage.size(), CV_32FC1);

    //【3】双层循环，遍历每一个像素点，改变map_x & map_y的值
    for (int j = 0; j < srcImage.rows; j++)
    {
        for (int i = 0; i < srcImage.cols; i++)
        {
            //改变map_x & map_y的值.
            map_x.at<float>(j, i) = static_cast<float>(i); // 行列倒置
            map_y.at<float>(j, i) = static_cast<float>(srcImage.rows - j);
        }
    }

    //【4】进行重映射操作
    remap(srcImage, dstImage, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));

    //【5】显示效果图
    imshow("【程序窗口】", dstImage);
    waitKey();

    return 0;
}