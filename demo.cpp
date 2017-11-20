
#include "gesture.hpp"

void DrawText(cv::Mat& img,std::string text,int x, int y,cv::Scalar color)
{
    cv::putText(img,text.c_str(),cv::Point(x,y),cv::FONT_HERSHEY_SIMPLEX,0.8,color,2,1);
}

int main()
{
    cv::VideoCapture cap(0);
    cv::Mat img;
    
    char ch;
    
    gesture* gs = new gesture;

    while(true)
    {
        cap>>img;

        gs->detect(img);
        if(gs->is_fist())
        {
            cv::Rect Fists = gs->get_fist();
            cv::putText(img,"Fist",cv::Point(Fists.x,Fists.y),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,0,255),2,1);
        }
        if(gs->is_palm())
        {
            cv::Rect palm = gs->get_palm();
            cv::putText(img,"Palm",cv::Point(palm.x,palm.y),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,0,255),2,1);
            cv::Point* p = gs->get_palm_center();
            if(p)
            {
                cv::circle(img, *p, 5, cv::Scalar(0,255,0), -1, 8, 0);
                if(gs->is_select_start())
                    cv::rectangle(img, *gs->get_select_lt(), *p, cv::Scalar(0,255,0), 1, 8, 0);
            }

        }
        if(gs->is_select_confirmed())
        {
            cv::rectangle(img, *gs->get_select_lt(), *gs->get_select_rb(), cv::Scalar(255,0,0), 5, 8, 0);
            gs->clear_select();
        }

        char fps_str[256];
        sprintf(fps_str,"%s %d","FPS : ",(int)gs->get_avg_fps());
        DrawText(img,fps_str,10,50,cv::Scalar(0,255,0));

        cv::imshow("Gesture Recognition",img);
        ch=cv::waitKey(1);
        if(ch==27)
        {
            break;
        }
    }

    delete gs;
    return 0;
}
