#include "gesture.hpp"

double gesture::get_avg_fps()
{
    return avg_fps;
}

cv::Rect gesture::get_fist()
{
    return Fists[0];
}

cv::Rect gesture::get_palm()
{
    return palm[0];
}

void gesture::detect(cv::Mat& img)
{
    int64 start = cv::getTickCount();

    s = cv::getTickCount();
    Palm.detectMultiScale(img,palm,1.1,2,0|CV_HAAR_SCALE_IMAGE,cv::Size(100,100));
    time_palm += (cv::getTickCount() - s) / cv::getTickFrequency();

    s = cv::getTickCount();
    Fist.detectMultiScale(img,Fists,1.1,2,0|CV_HAAR_SCALE_IMAGE,cv::Size(100,100));
    time_fist += (cv::getTickCount() - s) / cv::getTickFrequency();

    if(is_fist())
        add(G_FIST, &(Fists[0]));
    if(is_palm())
        add(G_PALM, &(palm[0]));

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
}

bool gesture::is_fist()
{
    return Fists.size() == 1;
}

bool gesture::is_palm()
{
    return palm.size() == 1;
}

bool gesture::is_select_start()
{
    return G_FIRST_CORD == m_state_cord;
}


cv::Point* gesture::get_select_lt()
{
    return &(e_gesture.point_lt);
}

cv::Point* gesture::get_select_rb()
{
    return &(e_gesture.point_rb);
}

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

void gesture::clear_select()
{
    m_state_cord = G_NONE;
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
