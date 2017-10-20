#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>
#include <stdio.h>

cv::Mat img;

std::vector<cv::Rect> palm;
std::vector<cv::Rect> Fists;

cv::CascadeClassifier Fist, Palm;

void DrawText(cv::Mat& img,std::string text,int x, int y,cv::Scalar color)
{
    cv::putText(img,text.c_str(),cv::Point(x,y),cv::FONT_HERSHEY_SIMPLEX,0.8,color,2,1);
}

int main()
{
    cv::VideoCapture cap(0);
    char ch;
    Fist.load("fist.xml");
    Palm.load("rpalm.xml");
    double time_past=0;
    int frame_cnt = 0;
    int T=5;
    double avg_fps=0;
    int64 s;
    double time_palm=0, time_fist=0;
    while(true)
    {
        int64 start = cv::getTickCount();
        cap>>img;

        s = cv::getTickCount();
        Palm.detectMultiScale(img,palm,1.1,2,0|CV_HAAR_SCALE_IMAGE,cv::Size(100,100));
        time_palm += (cv::getTickCount() - s) / cv::getTickFrequency();

        s = cv::getTickCount();
        Fist.detectMultiScale(img,Fists,1.1,2,0|CV_HAAR_SCALE_IMAGE,cv::Size(100,100));
        time_fist += (cv::getTickCount() - s) / cv::getTickFrequency();

        for(unsigned int	i=0 ,j=0; i< Fists.size() || j < palm.size() ; j++,i++)
        {
            if(Fists.size())
            {
                cv::putText(img,"Fist",cv::Point(Fists[i].x,Fists[i].y),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,0,255),2,1);
                cv::rectangle(img,cv::Point(Fists[i].x,Fists[i].y),cv::Point(Fists[i].x+ Fists[i].width,Fists[i].y+ Fists[i].height),cv::Scalar(0,255,255),1,8,0);
            }
            if(palm.size())
            {
                cv::putText(img,"Palm",cv::Point(palm[i].x,palm[i].y),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,0,255),2,1);
                cv::rectangle(img,cv::Point(palm[i].x,palm[i].y),cv::Point(palm[i].x+ palm[i].width,palm[i].y+ palm[i].height),cv::Scalar(0,255,255),1,8,0);
            }
        }
        frame_cnt++;
        time_past += (cv::getTickCount() - start) / cv::getTickFrequency();
        if(time_past >= T)
        {
            avg_fps = (double)frame_cnt / time_past;
            printf("average fps:%3.2f, time_palm:%f ms, time_fist:%f ms in %d second\n", avg_fps, time_palm/frame_cnt*1000, time_fist/frame_cnt*1000, T);
            frame_cnt = 0;
            time_past = 0;
            time_palm = 0;
            time_fist = 0;
        }
        char fps_str[256] ;
        sprintf(fps_str,"%s %d","FPS : ",(int)avg_fps);
        DrawText(img,fps_str,10,50,cv::Scalar(0,255,0));

        cv::imshow("Gesture Recognition",img);
        ch=cv::waitKey(1);
        if(ch==27)
        {
            break;
        }
    }
    return 0;
}
