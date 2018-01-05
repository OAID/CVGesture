#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <cstdlib>
#include <list>
#include <string>
#include <fstream>
#include <memory.h>
#include "json/json.h"

#define CONFIG_FILENAME "config.json"

void DrawText(cv::Mat& img,std::string text,int x, int y,cv::Scalar color)
{
    cv::putText(img,text.c_str(),cv::Point(x,y),cv::FONT_HERSHEY_SIMPLEX,0.8,color,2,1);
}

struct sys_config
{
    int palm_detect_neighbor_num;
    int palm_detect_size;
    int fist_detect_neighbor_num;
    int fist_detect_size;
    int iter_time;
};

bool parse_config(char * path, sys_config & config)
{
    std::ifstream ifs;
    Json::Reader reader;
    Json::Value root;

    ifs.open(path, std::ios::in | std::ios::binary);
    if (ifs.is_open() == false) {
		std::cout << "Open file failed!\n";
        return false;
    }

    if (!reader.parse(ifs, root, false)) {
		std::cout << "Read file failed!\n";
        ifs.close();
        return false;
    }

    memset(&config, 0, sizeof(sys_config));

    config.palm_detect_neighbor_num = root["palm detect neighbor number"].asInt();
    config.palm_detect_size = root["palm detect rectangle size"].asInt();

    config.fist_detect_neighbor_num = root["fist detect neighbor number"].asInt();
    config.fist_detect_size = root["fist detect rectangle size"].asInt();

    config.iter_time = root["test iteration time"].asInt();

    ifs.close();
        return true;
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

    sys_config config;
    

    double time_past=0;
    int frame_cnt = 0;
    int T=5;
    double avg_fps=0;
    int64 s;
    double time_palm=0, time_fist=0;
    int pdetect_num, fdetect_num;
    int pdetect_rec, fdetect_rec;
    
    if(parse_config((char *)CONFIG_FILENAME, config))
    {
        pdetect_num = config.palm_detect_neighbor_num;
        pdetect_rec = config.palm_detect_size;
        fdetect_num = config.fist_detect_neighbor_num;
        fdetect_rec = config.fist_detect_size;
        T = config.iter_time;
    }
    else
    {
        std::cout << "config error!" << std::endl;
    }

    while(true)
    {
        cap>>img;
	int64 start = cv::getTickCount();
	s = cv::getTickCount();
        Palm.detectMultiScale(img,prect,1.1,pdetect_num,0|CV_HAAR_SCALE_IMAGE,cv::Size(pdetect_rec,pdetect_rec));
	time_palm += (cv::getTickCount() - s) / cv::getTickFrequency();
	for (int i = 0; i < prect.size();i++){
	    cv::rectangle(img, prect[i],cv::Scalar(0,0,255),4); 
	    cv::putText(img,"Palm",cv::Point(prect[i].x,prect[i].y-10),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,0,255),2,1);	
	
	}
	s = cv::getTickCount();
	Fist.detectMultiScale(img,frect,1.1,fdetect_num,0|CV_HAAR_SCALE_IMAGE,cv::Size(fdetect_rec,fdetect_rec));
	time_fist += (cv::getTickCount() - s) / cv::getTickFrequency();
	for (int i = 0; i < frect.size();i++){
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
