
#include <stdio.h>
#include <opencv2/opencv.hpp>


#define COST_OBS_ROS 200	

#define COST_UNKNOWN_ROS 255		// 255 is unknown cost
#define COST_OBS 254		// 254 for forbidden regions

#define POT_HIGH 1.0e10	

using namespace cv;
using namespace std;
 
struct CellData{
CellData(unsigned int i,unsigned int x, unsigned int y,unsigned int sx, unsigned int sy) :
    index_(i) ,x(x), y(y),src_x(sx), src_y(sy)
    {
    }
    unsigned int index_;
    unsigned int x, y;
    unsigned int src_x, src_y;
};

Mat image = imread("1.png",IMREAD_GRAYSCALE);   
cv::Mat src(image.rows,image.cols,CV_8UC1);

cv::Mat costarr(image.rows,image.cols,CV_8UC1);

std::deque<CellData> inflation_queue;

std::deque<Point> openList;

bool* seen_ = new bool[image.rows*image.cols];
float* potarr = new float[image.rows*image.cols];

int main(int argc, char** argv)
{
    double color_avg,occ;
    unsigned char value;

    for(int i = 0; i < image.rows; i++) //遍历行
    {
      for (int j = 0; j < image.cols; j++) //遍历列
      {
        color_avg = image.at<uchar>( i, j );

        occ = (255 - color_avg) / 255.0;

        if( occ > 0.9 )
        {
          value = 254;
        }
        else if(occ < 0.55)
        {
          value = 0;
        }
        else 
        {
          value = -1;
        }

        src.at<uchar>( i, j ) = value;
      }
    } 
 

    inflation_queue.clear();
    memset(seen_, false, image.rows*image.cols * sizeof(bool));

    for(int i=1;i<src.rows-1;i++){ 
        for(int j=1;j<src.cols-1;j++){
            if(src.at<uchar>(i, j) > 0)
            {
                int u,d,l,r;
                if( src.at<uchar>(i, j) == 255)
                {
                    seen_[i*src.cols + j] = true;
                    continue;
                }

                l = src.at<uchar>(i, j-1);
                r = src.at<uchar>(i, j+1);		
                u = src.at<uchar>(i-1, j);
                d = src.at<uchar>(i+1, j);

                if(l == 0 || r == 0 || u == 0 || d == 0)
                {
                  inflation_queue.push_back(CellData((i*src.cols + j),i,j,i,j));
                }

                if(l > 0 && r > 0 && u > 0 && d > 0)
                {
                  seen_[i*src.cols + j] = true;
                }
            }
        }
    }


    while (!inflation_queue.empty())
    {
        const CellData& current_cell = inflation_queue.front();

        unsigned int index = current_cell.index_;
        unsigned int mx = current_cell.x;
        unsigned int my = current_cell.y;
        unsigned int sx = current_cell.src_x;
        unsigned int sy = current_cell.src_y;
        float distance = sqrt((mx-sx)*(mx-sx)+(my-sy)*(my-sy));

        inflation_queue.pop_front();

        if (seen_[index])
        {
            continue;
        }

        seen_[index] = true;

        unsigned char cost = 254 - (5*distance);
        src.at<uchar>(mx, my) = cost;

        if(mx > 0 && distance < 50 && !seen_[(mx - 1)*src.cols + my])
            inflation_queue.push_back(CellData(((mx - 1)*src.cols + my),mx - 1 ,my,sx,sy));

        if(my > 0 && distance < 50 && !seen_[mx*src.cols + my - 1])
            inflation_queue.push_back(CellData((mx*src.cols + my - 1),mx ,my - 1,sx,sy));

        if(mx < src.rows && distance < 50 && !seen_[(mx + 1)*src.cols + my])
            inflation_queue.push_back(CellData(((mx + 1)*src.cols + my),mx + 1 ,my,sx,sy));

        if(my < src.cols && distance < 50 && !seen_[mx*src.cols + my + 1])
            inflation_queue.push_back(CellData((mx*src.cols + my + 1),mx ,my + 1,sx,sy));
    }


    memset(seen_, false, image.rows*image.cols * sizeof(bool));

    for(int i = 0; i < src.rows; i++) //遍历行
    {
        for (int j = 0; j < src.cols; j++) //遍历列
        {
            costarr.at<uchar>( i, j ) = COST_OBS;

            int v = src.at<uchar>(i, j);
            if (v < COST_OBS_ROS)
            {
                v = 50 + 0.74*v;
                if (v >= COST_OBS_ROS)
                v = COST_OBS_ROS;

                costarr.at<uchar>( i, j ) = (unsigned char)v;
            }

            potarr[i*src.cols + j] = POT_HIGH;
        }
    } 
    


    Point start(397,579); 
    Point end(547,59); 

    potarr[start.y*costarr.cols + start.x] = 0;

    seen_[start.y*costarr.cols + start.x] = true;

    openList.push_back(start);

    

    while (!openList.empty())
    {
        Point open_point = openList.front();

        openList.pop_front();

        for(int x=open_point.x-1;x<=open_point.x+1;x++) 
        {
            for(int y=open_point.y-1;y<=open_point.y+1;y++) 
            {
                if(x >= 0 && x <= costarr.cols && y >= 0 && y <= costarr.rows && costarr.at<uchar>(Point(x, y)) < COST_OBS_ROS)
                {
                    if(seen_[y*costarr.cols + x])
                    {
                        if(potarr[open_point.y*costarr.cols + open_point.x] + costarr.at<uchar>(Point(x, y)) <= potarr[y*costarr.cols + x])
                        {
                            potarr[y*costarr.cols + x] = potarr[open_point.y*costarr.cols + open_point.x] + costarr.at<uchar>(Point(x, y)) ;
                        }
                    }
                    else
                    {
                        openList.push_back(Point(x,y));
                        seen_[y*costarr.cols + x] = true;
                        potarr[y*costarr.cols + x] = potarr[open_point.y*costarr.cols + open_point.x] + costarr.at<uchar>(Point(x, y)) ;
                    }
                }
            }
        }

        if(potarr[end.y*costarr.cols + end.x] < POT_HIGH)
        {
            cout<<"potarr end:"<< potarr[end.y*costarr.cols + end.x]<<endl;
            break;
        }
        //circle(image, Point(open_point.x,open_point.y), 0,Scalar(0,0,0),-1);
    }


    Point use_point = end;

    float dx=0;
    float dy=0;

    while (use_point != start)
    {
        int n = use_point.y*costarr.cols + use_point.x;

        if(potarr[n] < 50)
        {
            cout<<"potarr end:"<< potarr[end.y*costarr.cols + end.x]<<endl;
            break;
        }

        float cv = potarr[n];
        float dx = 0.0;
        float dy = 0.0;

        if (potarr[n-1] < POT_HIGH -1 )
        {
            dx += potarr[n-1]- cv;	
        }

        
          

        if (potarr[n+1] < POT_HIGH -1 )
        {
            dx += cv - potarr[n+1]; 
        }
  
        
          

        if (potarr[n-costarr.cols] < POT_HIGH - 1)
        {
            dy += potarr[n-costarr.cols]- cv;	
        }

          

        if (potarr[n+costarr.cols] < POT_HIGH -1 )
        {
            dy += cv - potarr[n+costarr.cols]; 
        }

        
          


        if(dx > 0)
        {
            use_point.x += 1;
        }

        if(dx < 0)
        {
            use_point.x -= 1;
        }


        if(dy > 0)
        {
            use_point.y += 1;
        }

        if(dy < 0)
        {
            use_point.y -= 1;
        }


        circle(costarr, Point(use_point.x,use_point.y), 0,Scalar(0,0,0),-1);

        imshow("costarr",costarr);

    
       waitKey(10);

       
    }

    //cout<<"potarr end:"<< potarr <<endl;


    imwrite("costarr.png", costarr);


    imshow("costarr",costarr);

    
       waitKey(0);


   
     

    

    

    

    return 0;

}

