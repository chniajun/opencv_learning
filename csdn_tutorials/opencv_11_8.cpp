//-----------------------------------【程序说明】----------------------------------------------
//		程序名称:：《【OpenCV入门教程之十一】形态学图像处理（一）：膨胀与腐蚀  》 博文配套源码
//		开发所用IDE版本：Visual Studio 2010
//   		开发所用OpenCV版本：	2.4.8
//		2014年4月25日 Create by 浅墨
//----------------------------------------------------------------------------------------------

//-----------------------------------【头文件包含部分】---------------------------------------
//		描述：包含程序所依赖的头文件
//----------------------------------------------------------------------------------------------
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//-----------------------------------【命名空间声明部分】--------------------------------------
//		描述：包含程序所使用的命名空间
//-----------------------------------------------------------------------------------------------
using namespace std;
using namespace cv;


// 而膨胀就是求局部最大值的操作   白色区域扩大  
// 腐蚀就是求局部最小值的操作     白色区域减小

// 开运算可以用来               先腐蚀再膨胀
         //消除白色小物体
         //在纤细点处分离物体
         //平滑较大物体的边界的同时并不明显改变其面积。


// 闭运算能够                   先膨胀再腐蚀
         //排除小型空洞（指黑色区域）
         //平滑物体轮廓弥合（连接）窄的间断点
         //沟壑 填补轮廓线断裂   


// 形态学梯度                   膨胀图与腐蚀图之差
         //突出高亮区域的外围
         //为轮廓查找提供新思路             


// 顶帽运算                     原图像与“开运算“的结果图之差
         //可以使用顶帽运算进行背景提取


// 黑帽运算                     闭运算的结果图与原图像之差
         //突出了与原图像轮廓周围的区域更暗的区域





//-----------------------------------【全局变量声明部分】--------------------------------------
//		描述：全局变量声明
//-----------------------------------------------------------------------------------------------
Mat g_srcImage, g_dstImage;       //原始图和效果图
int g_nElementShape = MORPH_RECT; //元素结构的形状

//变量接收的TrackBar位置参数
int g_nMaxIterationNum = 10;
int g_nOpenCloseNum = 0;
int g_nErodeDilateNum = 0;
int g_nTopBlackHatNum = 0;

//-----------------------------------【全局函数声明部分】--------------------------------------
//		描述：全局函数声明
//-----------------------------------------------------------------------------------------------
static void on_OpenClose(int, void *);   //回调函数
static void on_ErodeDilate(int, void *); //回调函数
static void on_TopBlackHat(int, void *); //回调函数
static void ShowHelpText();              //帮助文字显示

//-----------------------------------【main( )函数】--------------------------------------------
//		描述：控制台应用程序的入口函数，我们的程序从这里开始
//-----------------------------------------------------------------------------------------------
int main()
{
    ShowHelpText();

    //载入原图
    g_srcImage = imread("13.png"); //工程目录下需要有一张名为1.jpg的素材图
    if (!g_srcImage.data)
    {
        printf("Oh，no，读取srcImage错误~！ \n");
        return false;
    }

    //显示原始图
    namedWindow("【原始图】");
    imshow("【原始图】", g_srcImage);

    //创建三个窗口
    namedWindow("【开运算/闭运算】", 1);
    namedWindow("【腐蚀/膨胀】", 1);
    namedWindow("【顶帽/黑帽】", 1);

    //参数赋值
    g_nOpenCloseNum = 9;
    g_nErodeDilateNum = 9;
    g_nTopBlackHatNum = 2;

    //分别为三个窗口创建滚动条
    createTrackbar("迭代值", "【开运算/闭运算】", &g_nOpenCloseNum, g_nMaxIterationNum * 2 + 1, on_OpenClose);
    createTrackbar("迭代值", "【腐蚀/膨胀】", &g_nErodeDilateNum, g_nMaxIterationNum * 2 + 1, on_ErodeDilate);
    createTrackbar("迭代值", "【顶帽/黑帽】", &g_nTopBlackHatNum, g_nMaxIterationNum * 2 + 1, on_TopBlackHat);

    //轮询获取按键信息
    while (1)
    {
        int c;

        //执行回调函数
        on_OpenClose(g_nOpenCloseNum, 0);
        on_ErodeDilate(g_nErodeDilateNum, 0);
        on_TopBlackHat(g_nTopBlackHatNum, 0);

        //获取按键
        c = waitKey(0);

        //按下键盘按键Q或者ESC，程序退出
        if ((char)c == 'q' || (char)c == 27)
            break;
        //按下键盘按键1，使用椭圆(Elliptic)结构元素结构元素MORPH_ELLIPSE
        if ((char)c == 49) //键盘按键1的ASII码为49
            g_nElementShape = MORPH_ELLIPSE;
        //按下键盘按键2，使用矩形(Rectangle)结构元素MORPH_RECT
        else if ((char)c == 50) //键盘按键2的ASII码为50
            g_nElementShape = MORPH_RECT;
        //按下键盘按键3，使用十字形(Cross-shaped)结构元素MORPH_CROSS
        else if ((char)c == 51) //键盘按键3的ASII码为51
            g_nElementShape = MORPH_CROSS;
        //按下键盘按键space，在矩形、椭圆、十字形结构元素中循环
        else if ((char)c == ' ')
            g_nElementShape = (g_nElementShape + 1) % 3;
    }

    return 0;
}

//-----------------------------------【on_OpenClose( )函数】----------------------------------
//		描述：【开运算/闭运算】窗口的回调函数
//-----------------------------------------------------------------------------------------------
static void on_OpenClose(int, void *)
{
    //偏移量的定义
    int offset = g_nOpenCloseNum - g_nMaxIterationNum;   //偏移量
    int Absolute_offset = offset > 0 ? offset : -offset; //偏移量绝对值
    //自定义核
    Mat element = getStructuringElement(g_nElementShape, Size(Absolute_offset * 2 + 1, Absolute_offset * 2 + 1), Point(Absolute_offset, Absolute_offset));
    //进行操作
    if (offset < 0)
        morphologyEx(g_srcImage, g_dstImage, CV_MOP_OPEN, element);
    else
        morphologyEx(g_srcImage, g_dstImage, CV_MOP_CLOSE, element);
    //显示图像
    imshow("【开运算/闭运算】", g_dstImage);
}

//-----------------------------------【on_ErodeDilate( )函数】----------------------------------
//		描述：【腐蚀/膨胀】窗口的回调函数
//-----------------------------------------------------------------------------------------------
static void on_ErodeDilate(int, void *)
{
    //偏移量的定义
    int offset = g_nErodeDilateNum - g_nMaxIterationNum; //偏移量
    int Absolute_offset = offset > 0 ? offset : -offset; //偏移量绝对值
    //自定义核
    Mat element = getStructuringElement(g_nElementShape, Size(Absolute_offset * 2 + 1, Absolute_offset * 2 + 1), Point(Absolute_offset, Absolute_offset));
    //进行操作
    if (offset < 0)
        erode(g_srcImage, g_dstImage, element);
    else
        dilate(g_srcImage, g_dstImage, element);
    //显示图像
    imshow("【腐蚀/膨胀】", g_dstImage);
}

//-----------------------------------【on_TopBlackHat( )函数】--------------------------------
//		描述：【顶帽运算/黑帽运算】窗口的回调函数
//----------------------------------------------------------------------------------------------
static void on_TopBlackHat(int, void *)
{
    //偏移量的定义
    int offset = g_nTopBlackHatNum - g_nMaxIterationNum; //偏移量
    int Absolute_offset = offset > 0 ? offset : -offset; //偏移量绝对值
    //自定义核
    Mat element = getStructuringElement(g_nElementShape, Size(Absolute_offset * 2 + 1, Absolute_offset * 2 + 1), Point(Absolute_offset, Absolute_offset));
    //进行操作
    if (offset < 0)
        morphologyEx(g_srcImage, g_dstImage, MORPH_TOPHAT, element);
    else
        morphologyEx(g_srcImage, g_dstImage, MORPH_BLACKHAT, element);
    //显示图像
    imshow("【顶帽/黑帽】", g_dstImage);
}

//-----------------------------------【ShowHelpText( )函数】----------------------------------
//		描述：输出一些帮助信息
//----------------------------------------------------------------------------------------------
static void ShowHelpText()
{
    //输出一些帮助信息
    printf("\n\t请调整滚动条观察图像效果~\n");
    printf("\n\t 9是中间值，小于9是另一种操作~\n");
    printf("\n\n\t按键操作说明: \n\n"
           "\t\t键盘按键【ESC】或者【Q】- 退出程序\n"
           "\t\t键盘按键【1】- 使用椭圆(Elliptic)结构元素\n"
           "\t\t键盘按键【2】- 使用矩形(Rectangle )结构元素\n"
           "\t\t键盘按键【3】- 使用十字型(Cross-shaped)结构元素\n"
           "\t\t键盘按键【空格SPACE】- 在矩形、椭圆、十字形结构元素中循环\n"
           "\n\n\t\t\t\t\t\t\t\t by浅墨");
}