#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <cstdlib>
#include <list>

using namespace std;

cv::Mat img;

std::vector<cv::Rect> palm;
std::vector<cv::Rect> Fists;

cv::CascadeClassifier Fist, Palm;

#define G_Q_SIZE 100

enum G_TYPE
{
    G_PALM,
    G_FIST,
};

enum STATE_G
{
    S_NONE,
    S_PALM,
    S_CLICK,
};

enum STATE_CORD
{
    G_NONE,
    G_FIRST_CORD,
    G_SELECT_CONFIRMING,
};

struct gesture_rec
{
    G_TYPE type;
    double time;
    cv::Rect rect;
    cv::Point center;
    bool is_click;
};

struct gesture_event
{
    cv::Point point_lt;
    cv::Point point_rb;
    cv::Rect rect_select;
};

class gesture
{
public:
    list<gesture_rec> l_gesture;
    gesture_event e_gesture;
    int64 m_tick_s;
    STATE_G m_state_g;
    double m_CLICK_JUDGE;
    STATE_CORD m_state_cord;
    bool is_click;
    double m_time_confirming;
    double m_TIME_CONFIRMING_JUDGE;
    unsigned int m_ANTI_SHAKING_CNT;

    gesture()
    {
        m_tick_s = cv::getTickCount();
        m_state_g = S_NONE;
        m_CLICK_JUDGE = 0.5;
        m_state_cord = G_NONE;
        m_TIME_CONFIRMING_JUDGE = 3;
        m_ANTI_SHAKING_CNT = 3;
    }

    ~gesture(){}

    void add(G_TYPE type, cv::Rect* rect);
    STATE_G update_state_g();
    void update_state_cord();
    cv::Point* get_palm_center();
    bool is_select_confirmed();
    double get_cur_time();
    STATE_G anti_shaking_state();
    bool anti_shaking_state(G_TYPE s);
    cv::Point find_gesture(G_TYPE dst_type);
};

cv::Point gesture::find_gesture(G_TYPE dst_type)
{
    list<gesture_rec>::reverse_iterator it;
    for(it=l_gesture.rbegin();it!=l_gesture.rend();it++)
    {
        if(it->type == dst_type)
            return it->center;
    }
    return {0,0};
}

bool gesture::anti_shaking_state(G_TYPE s)
{
    if(l_gesture.size()<m_ANTI_SHAKING_CNT)
        return false;

    list<gesture_rec>::reverse_iterator it;
    for(unsigned int i=0;i<m_ANTI_SHAKING_CNT;i++)
    {
        it = l_gesture.rbegin();
        if(it->type != s)
            return false;
        it++;
    }
    return true;
}

bool gesture::is_select_confirmed()
{
    double cur_time = get_cur_time();
    if(G_SELECT_CONFIRMING == m_state_cord && 
        cur_time - m_time_confirming > m_TIME_CONFIRMING_JUDGE)
        return 1;
    return 0;
}

cv::Point* gesture::get_palm_center()
{
    list<gesture_rec>::reverse_iterator it = l_gesture.rbegin();
    
    if(G_PALM == it->type)
        return &(it->center);
    else
        return NULL;
}

double gesture::get_cur_time()
{
    return (cv::getTickCount() - m_tick_s) / cv::getTickFrequency();
}

void gesture::add(G_TYPE type, cv::Rect* rect)
{
    gesture_rec rec;
    rec.type = type;
    rec.time = get_cur_time();
    rec.rect = *rect;
    rec.center.x = rect->x + rect->width/2;
    rec.center.y = rect->y + rect->height/2;

    l_gesture.push_back(rec);
    if (l_gesture.size() > G_Q_SIZE)
    {
        l_gesture.pop_front();
    }
    update_state_g();
}

STATE_G gesture::update_state_g()
{
    G_TYPE type;
    double time=1000;
    list<gesture_rec>::reverse_iterator it = l_gesture.rbegin();
    type = it->type;

    if(l_gesture.size()>=2)
    {
        time = it->time;
        it++;
        time -= it->time;
    }

    if(S_NONE == m_state_g && G_PALM == type && anti_shaking_state(G_PALM))
    {
        m_state_g = S_PALM;
    }
    else if(S_PALM == m_state_g && G_FIST == type && anti_shaking_state(G_FIST))
    {
        if(time < m_CLICK_JUDGE)
            m_state_g = S_CLICK;
        else
            m_state_g = S_NONE;
        
    }
    else if(S_CLICK == m_state_g)
        m_state_g = S_NONE;

    if(S_CLICK == m_state_g)
    {
        l_gesture.rbegin()->is_click = (m_state_g == S_CLICK);
    }

    update_state_cord();

    return m_state_g;
}

void gesture::update_state_cord()
{
    if(G_NONE == m_state_cord && S_CLICK == m_state_g)
    {
        e_gesture.point_lt = l_gesture.rbegin()->center;
        m_state_cord = G_FIRST_CORD;
    }
    else if(G_FIRST_CORD == m_state_cord && S_CLICK == m_state_g)
    {
        e_gesture.point_rb = l_gesture.rbegin()->center;
        e_gesture.rect_select.x = e_gesture.point_lt.x;
        e_gesture.rect_select.y = e_gesture.point_lt.y;
        e_gesture.rect_select.width = e_gesture.point_rb.x - e_gesture.point_lt.x;
        e_gesture.rect_select.height = e_gesture.point_rb.y - e_gesture.point_lt.y;
        m_state_cord = G_SELECT_CONFIRMING;
        m_time_confirming = l_gesture.rbegin()->time;
    }
    else if(G_SELECT_CONFIRMING == m_state_cord)
    {
    }
}

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
    gesture* gs = new gesture;

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

        unsigned int i=0;
        {
            if(Fists.size())
            {
                cv::putText(img,"Fist",cv::Point(Fists[i].x,Fists[i].y),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,0,255),2,1);
                // cv::rectangle(img,cv::Point(Fists[i].x,Fists[i].y),cv::Point(Fists[i].x+ Fists[i].width,Fists[i].y+ Fists[i].height),cv::Scalar(0,255,255),1,8,0);
                gs->add(G_FIST, &(Fists[i]));
            }
            if(palm.size())
            {
                cv::putText(img,"Palm",cv::Point(palm[i].x,palm[i].y),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,0,255),2,1);
                // cv::rectangle(img,cv::Point(palm[i].x,palm[i].y),cv::Point(palm[i].x+ palm[i].width,palm[i].y+ palm[i].height),cv::Scalar(0,255,255),1,8,0);
                gs->add(G_PALM, &(palm[i]));
                cv::Point* p = gs->get_palm_center();
                if(p)
                {
                    cv::circle(img, *p, 5, cv::Scalar(0,255,0), -1, 8, 0);
                    if(G_FIRST_CORD == gs->m_state_cord)
                        cv::rectangle(img, gs->e_gesture.point_lt, *p, cv::Scalar(0,255,0), 1, 8, 0);
                }

            }
            if(gs->is_select_confirmed())
                cv::rectangle(img, gs->e_gesture.point_lt, gs->e_gesture.point_rb, cv::Scalar(255,0,0), 5, 8, 0);
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

    delete gs;
    return 0;
}
