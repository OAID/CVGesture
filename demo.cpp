#include "gesture.hpp"

void DrawText(cv::Mat& img,std::string text,int x, int y,cv::Scalar color)
{
    cv::putText(img,text.c_str(),cv::Point(x,y),cv::FONT_HERSHEY_SIMPLEX,0.8,color,2,1);
}

int main()
{
    cv::VideoCapture cap(0);
    cv::Mat img;
    std::vector<cv::Rect> prect;
    std::vector<cv::Rect> frect;
    
    char ch;
    
    gesture* gs = new gesture;
    gs->read_config();

    while(true)
    {
        cap>>img;

        gs->detect(img);
        

        char fps_str[256];
        sprintf(fps_str,"%s %d","FPS : ",(int)gs->get_avg_fps());
        DrawText(img,fps_str,10,50,cv::Scalar(0,255,0));

        cv::imshow("Gesture Recognition",img);
        ch=cv::waitKey(1);
    if(ch=='s')
    {
      cv::imwrite("scr.jpg",img);
    }
        if(ch==27)
        {
            break;
        }
    }

    delete gs;
    return 0;
}