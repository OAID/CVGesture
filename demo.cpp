#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <cstdlib>
#include <list>

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
    cv::CascadeClassifier Fist, Palm;
    Fist.load("fist_v3.xml");
    Palm.load("palm_v4.xml");

    double time_past=0;
    int frame_cnt = 0;
    int T=5;
    double avg_fps=0;
    int64 s;
    double time_palm=0, time_fist=0;

    while(true)
    {
        cap>>img;
	int64 start = cv::getTickCount();
	s = cv::getTickCount();
        Palm.detectMultiScale(img,prect,1.1,7,0|CV_HAAR_SCALE_IMAGE,cv::Size(90,90));
	time_palm += (cv::getTickCount() - s) / cv::getTickFrequency();
	for (unsigned int i = 0; i < prect.size();i++){
	    cv::rectangle(img, prect[i],cv::Scalar(0,0,255),4); 
	    cv::putText(img,"Palm",cv::Point(prect[i].x,prect[i].y-10),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,0,255),2,1);	
	
	}
	s = cv::getTickCount();
	Fist.detectMultiScale(img,frect,1.1,7,0|CV_HAAR_SCALE_IMAGE,cv::Size(80,80));
	time_fist += (cv::getTickCount() - s) / cv::getTickFrequency();
	for (unsigned int i = 0; i < frect.size();i++){
	    cv::rectangle(img, frect[i],cv::Scalar(0,255,0),4);
	    cv::putText(img,"Fist",cv::Point(frect[i].x,frect[i].y-10),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,255,0),2,1);	
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
	char fps_str[256];
        sprintf(fps_str,"%s %d","FPS : ",(int)avg_fps);
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

    return 0;
}
