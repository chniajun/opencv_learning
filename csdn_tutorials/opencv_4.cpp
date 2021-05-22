#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

//----------------------------------【ROI_AddImage( )函数】----------------------------------
// 函数名：ROI_AddImage（）
//     描述：利用感兴趣区域ROI实现图像叠加
//----------------------------------------------------------------------------------------------
bool ROI_AddImage()
{
    //【1】读入图像
    Mat srcImage1 = imread("dota_pa.jpg");
    Mat logoImage = imread("dota_logo.jpg");

    //【2】定义一个Mat类型并给其设定ROI区域
    Mat imageROI = srcImage1(Rect(10, 10, logoImage.cols, logoImage.rows));

    //【3】加载掩模（必须是灰度图）
    Mat mask = imread("dota_logo.jpg", 0);
    imshow("窗口2", srcImage1);
    imshow("窗口2", mask);
    //【4】将掩膜拷贝到ROI
    //logoImage.copyTo(imageROI);//，作用是把image的内容粘贴到imageROI；

    //对于函数 srcImage.copyto(dstImage, mask) ，mask作为一个掩模板，
    //如果在某个像素点(i, j)其值为1（只看第一通道，所以mask单通道即可）
    //则把srcImage.at(i, j)处的值直接赋给dstImage.at(i, j)，
    //如果其值为0则dstImage.at(i, j)处保留其原始像素值。

    logoImage.copyTo(imageROI, mask);

    //【5】显示结果
    namedWindow("<1>利用ROI实现图像叠加示例窗口");
    imshow("<1>利用ROI实现图像叠加示例窗口", srcImage1);

    return true;
}

//---------------------------------【LinearBlending（）函数】-------------------------------------
// 函数名：LinearBlending（）
// 描述：利用cv::addWeighted（）函数实现图像线性混合
//--------------------------------------------------------------------------------------------
bool LinearBlending()
{
       //【0】定义一些局部变量
       double alphaValue = 0.5;
       double betaValue;
       Mat srcImage2, srcImage3, dstImage;
 
       //【1】读取图像 ( 两幅图片需为同样的类型和尺寸 )
       srcImage2= imread("mogu.jpg");
       srcImage3= imread("rain.jpg");
 
       if(!srcImage2.data ) { printf("你妹，读取srcImage2错误~！ \n"); return false; }
       if(!srcImage3.data ) { printf("你妹，读取srcImage3错误~！ \n"); return false; }
 
       //【2】做图像混合加权操作
       betaValue= ( 1.0 - alphaValue );
       addWeighted(srcImage2, alphaValue, srcImage3, betaValue, 0.0, dstImage);
 
       //【3】创建并显示原图窗口
       namedWindow("<2>线性混合示例窗口【原图】 by浅墨", 1);
       imshow("<2>线性混合示例窗口【原图】 by浅墨", srcImage2 );
 
       namedWindow("<3>线性混合示例窗口【效果图】 by浅墨", 1);
       imshow("<3>线性混合示例窗口【效果图】 by浅墨", dstImage );
 
       return true;
      
}

int main()
{
    //ROI_AddImage();
    LinearBlending();
    waitKey();
    return 0;
}