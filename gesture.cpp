#include "gesture.hpp"

double gesture::get_avg_fps()
{
    return avg_fps;
}

cv::Rect gesture::get_fist()
{
    return fist[0];
}

cv::Rect gesture::get_palm()
{
    return palm[0];
}

bool gesture::parse_config(char * path, sys_config & config)
{
    std::ifstream ifs;
    Json::Reader reader;
    Json::Value root;
    Json::Value libApis;

    ifs.open(path, std::ios::in | std::ios::binary);
    if (ifs.is_open() == false) {
        log_error("Open file failed!\n");
        return false;
    }

    if (!reader.parse(ifs, root, false)) {
        log_error("Read file failed!\n");
        ifs.close();
        return false;
    }

    libApis = root["APIs"];

    //memset(&config, 0, sizeof(sys_config));
    config.palm_path = root["palm_classifier_path"].asString();
    config.fist_path = root["fist_classifier_path"].asString();

    config.palm_detect_neighbor_num = libApis[0]["palm detect neighbor number"].asInt();
    config.palm_detect_size = libApis[0]["palm detect rectangle size"].asInt();

    config.fist_detect_neighbor_num = libApis[0]["fist detect neighbor number"].asInt();
    config.fist_detect_size = libApis[0]["fist detect rectangle size"].asInt();

    config.iter_time = libApis[0]["test iteration time"].asInt();
    
    config.click_judge_time = libApis[0]["click judge time"].asDouble();
    config.confirm_time = libApis[0]["draw rectangle delay"].asDouble();
    config.anti_shake_cnt = libApis[0]["anti-shake frame count"].asInt();

    log_printf("---------------Config Json file loaded---------------\n");
    log_printf("Palm classifier path: %s\n",config.palm_path.data());
    log_printf("Fist classifier path: %s\n",config.fist_path.data());
    log_printf("Palm detect neighbor number: %d\n",config.palm_detect_neighbor_num);
    log_printf("Palm detect rectangle size: %d\n",config.palm_detect_size);
    log_printf("Fist detect neighbor number: %d\n",config.fist_detect_neighbor_num);
    log_printf("Fist detect rectangle size: %d\n",config.fist_detect_size);
    log_printf("Test iteration time: %d\n",config.iter_time);
    log_printf("-----------------------------------------------------\n");


    ifs.close();
    return true;
}

void gesture::read_config()
{
    if(parse_config((char *)CONFIG_FILENAME, config))
    {
        pdetect_num = config.palm_detect_neighbor_num;
        pdetect_rec = config.palm_detect_size;
        fdetect_num = config.fist_detect_neighbor_num;
        fdetect_rec = config.fist_detect_size;
        T = config.iter_time;
        m_CLICK_JUDGE = config.click_judge_time;
        m_TIME_CONFIRMING_JUDGE = config.confirm_time;
        m_ANTI_SHAKING_CNT = config.anti_shake_cnt;
        fist_path = config.fist_path;
        palm_path = config.palm_path;
        Fist.load(fist_path);
        Palm.load(palm_path);
    }
    else
    {
        log_error("config read error! All parameters are set to default value.\n");
        pdetect_num = 7;
        pdetect_rec = 90;
        fdetect_num = 7;
        fdetect_rec = 80;
        T = 5;
        m_CLICK_JUDGE = 0.5;
        m_TIME_CONFIRMING_JUDGE = 0.2;
        m_ANTI_SHAKING_CNT = 4;
        fist_path = "palm_v4.xml";
        palm_path = "fist_v3.xml";
        Fist.load(fist_path);
        Palm.load(palm_path);
    }
}


void gesture::detect(cv::Mat& img)
{
    img.copyTo(cam_img);
    split(cam_img, imageRGB);  
    for (int i = 0; i < 3; i++)  
    {  
	   equalizeHist(imageRGB[i], imageRGB[i]);  
    }  
    merge(imageRGB, 3, cam_img);
    //int64 start = cv::getTickCount();

    //s = cv::getTickCount();
    t.start();
    s = t.gettimegap();
    Palm.detectMultiScale(cam_img,palm,1.1,pdetect_num,0|CV_HAAR_SCALE_IMAGE,cv::Size(pdetect_rec,pdetect_rec));
    Fist.detectMultiScale(cam_img,fist,1.1,fdetect_num,0|CV_HAAR_SCALE_IMAGE,cv::Size(fdetect_rec,fdetect_rec));
    //time_detect += (cv::getTickCount() - s) / cv::getTickFrequency();
    t.pause();
    time_detect += (t.gettimegap() - s)/1000;
    t.recovery();
    for (unsigned int i = 0; i < palm.size();i++){
        x1i = palm[i].x;
        y1i = palm[i].y;
        x2i = x1i + palm[i].width;
        y2i = y1i + palm[i].height;
        for (unsigned int j = 0; j < fist.size();j++){
            iou = 0.0;
            x1j = fist[j].x;
            y1j = fist[j].y;
            x2j = x1j + fist[j].width;
            y2j = y1j + fist[j].height;
            areai = (x2i-x1i+1)*(y2i-y1i+1);
            areaj = (x2j-x1j+1)*(y2j-y1j+1);

            xx1 = max(x1i, x1j);
            yy1 = max(y1i, y1j);
            xx2 = min(x2i, x2j);
            yy2 = min(y2i, y2j);

            h = max(0.0, yy2-yy1+1);
            w = max(0.0, xx2-xx1+1);

            intersection = w * h;
            iou = max(iou,(intersection / (areai + areaj - intersection)));
            //printf("iou is: %f\n",iou);
            if (iou>=0.5)
            {
                std::vector<cv::Rect>::iterator pnlist = fist.begin();
                advance(pnlist,j);
                if(pnlist!=fist.end())
                {
                    fist.erase(pnlist);
                    j-=1;
                }
            }
        }
    }
    
    for (unsigned int i = 0; i < palm.size();i++){
        cv::rectangle(img, palm[i],cv::Scalar(0,0,255),4); 
        cv::putText(img,"Palm",cv::Point(palm[i].x,palm[i].y-10),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,0,255),2,1); 
    }
    
    //palm_tlist.push_back((cv::getTickCount() - s) / cv::getTickFrequency());
    
    for (unsigned int i = 0; i < fist.size();i++){
        cv::rectangle(img, fist[i],cv::Scalar(0,255,0),4);
        cv::putText(img,"Fist",cv::Point(fist[i].x,fist[i].y-10),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,255,0),2,1); 
    }
    
    //fist_tlist.push_back((cv::getTickCount() - s) / cv::getTickFrequency());
    /*
    if(is_palm())
        add(G_PALM, &(palm[0]));
    else if(is_fist())
        add(G_FIST, &(fist[0]));
    
    frame_cnt++;
    time_past += (cv::getTickCount() - start) / cv::getTickFrequency();
    
    if(palm_tlist.size()>=100){
        time_palm = 0;
        list<double>::iterator pi;
        for (pi = palm_tlist.begin(); pi != palm_tlist.end(); ++pi)
            time_palm+=*pi;
        printf("average time_palm per 100 frames:%f ms\n", time_palm/100*1000);
        palm_tlist.clear();
    }
    if(fist_tlist.size()>=100){
        time_fist = 0;
        list<double>::iterator fi;
        for (fi = fist_tlist.begin(); fi != fist_tlist.end(); ++fi)
            time_fist+=*fi;
        printf("average time_fist per 100 frames:%f ms\n", time_fist/100*1000);
        fist_tlist.clear();
    }
    */
    t.pause();
    time_past+=(t.gettimegap()-s)/1000;
    frame_cnt++;
    if(time_past/1000 >= T)
    {
        avg_fps = (double)frame_cnt / time_past * 1000;
        log_printf("average fps:%3.2f, detection time:%f ms in %d second\n", avg_fps, time_detect/frame_cnt, T);
        frame_cnt = 0;
        time_past = 0;
        time_detect = 0;
    }
    t.recovery();
}

bool gesture::is_fist()
{
    return fist.size() >= 1;
}

bool gesture::is_palm()
{
    return palm.size() >= 1;
}

/*
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
*/
