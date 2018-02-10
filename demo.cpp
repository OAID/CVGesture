#include "gesture.hpp"
using namespace cv;

void DrawText(Mat& img,std::string text,int x, int y,Scalar color)
{
    putText(img,text.c_str(),Point(x,y),FONT_HERSHEY_SIMPLEX,0.8,color,2,1);
}

int main()
{
    VideoCapture cap(0);
    Mat img; 
    std::vector<Rect> prect;
    std::vector<Rect> frect;
    
    char ch;
    
    gesture* gs = new gesture;
    gs->read_config();

    while(true)
    {
        cap>>img;
        gs->detect(img);
        
        char fps_str[256];
        sprintf(fps_str,"%s %d","FPS : ",(int)gs->get_avg_fps());
        DrawText(img,fps_str,10,50,Scalar(0,255,0));

        imshow("Gesture Recognition",img);
        ch=waitKey(1);
    if(ch=='s')
    {
        imwrite("scr.jpg",img);
    }
        if(ch==27)
        {
            break;
        }
    }

    delete gs;
    return 0;
}
