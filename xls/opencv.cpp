
#include <opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
 
using namespace cv;

int main (void)
{
 Mat girl=imread("girl.jpg"); //载入图像到Mat
namedWindow("【1】动漫图"); //创建一个名为 "【1】动漫图"的窗口 
imshow("【1】动漫图",girl);//显示名为 "【1】动漫图"的窗口

 waitKey();
 return 0;
}
