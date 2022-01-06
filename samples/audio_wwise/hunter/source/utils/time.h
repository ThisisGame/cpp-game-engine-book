//
// Created by captain on 2021/8/5.
//

#ifndef UNTITLED_TIME_H
#define UNTITLED_TIME_H


#include <string>
#include <chrono>

class Time
{
public:
    Time();
    ~Time();

    //初始化
    static void Init();

    static void Update();

    //获取游戏运行时间
    static float TimeSinceStartup();

    static float delta_time(){return delta_time_;}

private:
    static std::chrono::system_clock::time_point startup_time_;
    static float last_frame_time_;
    static float delta_time_;//上一帧花费的时间
};


#endif //UNTITLED_TIME_H
