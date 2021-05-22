
#include <stdio.h>
#include <opencv2/opencv.hpp>


#define COST_OBS_ROS 200	

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

typedef struct UseList
{
	Point current_point;
    double gradient;
	UseList* father;

	UseList(Point point)
	{
		this->current_point = point;
		this->gradient = 0 ;
		this->father = NULL;
	}
	UseList(Point point,double gradient,UseList* father)
	{
		this->current_point = point;
		this->gradient = gradient;
		this->father = father;
	}
}UseList;


int main(int argc, char** argv)
{
    Mat image = imread("1.png",IMREAD_GRAYSCALE);   
    cv::Mat src(image.rows,image.cols,CV_8UC1);
    cv::Mat gravitation;

    gravitation = image.clone();

    std::deque<CellData> inflation_queue;

    bool* seen_ = new bool[image.rows*image.cols];
    memset(seen_, false, image.rows*image.cols * sizeof(bool));

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

    for(int i = 0; i < src.rows; i++) //遍历行
    {
      for (int j = 0; j < src.cols; j++) //遍历列
      {
        if(src.at<uchar>( i, j ) >= COST_OBS_ROS)
          src.at<uchar>( i, j ) = -1;

        if(src.at<uchar>( i, j ) == 0)
          src.at<uchar>( i, j ) = 3;
      }
    } 

    vector<UseList*> openList;

    Point start(397,579); 
    Point end(547,59); 

    UseList *startPos = new UseList(start);
	UseList *endPos = new UseList(end);

    UseList *open_point;


    openList.push_back(startPos);

    memset(seen_, false, image.rows*image.cols * sizeof(bool));

    seen_[start.y*src.cols + start.x] = true;

    while (!openList.empty())
    {
        open_point = openList[0];

        openList.erase(openList.begin());

        for(int x=open_point->current_point.x-1;x<=open_point->current_point.x+1;x++) 
        {
            for(int y=open_point->current_point.y-1;y<=open_point->current_point.y+1;y++) 
            {
                if(x >= 0 && x <= src.cols && y >= 0 && y <= src.rows && src.at<uchar>(Point(x, y)) < COST_OBS_ROS)
                {
                    UseList * point;
                    if(seen_[y*src.cols + x])
                    {
                        for (int i = 0;i < openList.size();i++)
                        {
                            if (openList.at(i)->current_point.x == x && openList.at(i)->current_point.y == y)
                            {
                                if((open_point->gradient  +  src.at<uchar>(Point(x, y)) )  <=  openList.at(i)->gradient)
                                {
                                    openList.at(i)->father = open_point;
                                    openList.at(i)->gradient = open_point->gradient + src.at<uchar>(Point(x, y));
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        point = new UseList(Point(x,y), (open_point->gradient +  src.at<uchar>(Point(x, y)) ),open_point);
                        openList.push_back(point);
                        seen_[y*src.cols + x] = true;
                    }
                }
            }
        }

        if(open_point->current_point.x == end.x && open_point->current_point.y == end.y)
          break;

        circle(image, Point(open_point->current_point.x,open_point->current_point.y), 3,Scalar(0,0,0),-1);


        

    }

    
    

    while (open_point->father != NULL )
    {
      
        circle(src, Point(open_point->current_point.x,open_point->current_point.y), 0,Scalar(255,255,255),-1);

        open_point = open_point->father;

    }


    imshow("src",src);

    imwrite("src.png", src);

    waitKey(0);

    return 0;

    

    imshow("xls",image);
    imshow("src",src);
    imshow("gravitation",gravitation);

    
    waitKey(0);

    

    return 0;

}

