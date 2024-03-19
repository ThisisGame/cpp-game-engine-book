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

    //~zh 获取游戏运行时间
    //~en Get the game running time
    static float TimeSinceStartup();

    static float delta_time(){return delta_time_;}

    static float fixed_update_time(){return fixed_update_time_;}

    //~zh 设置固定更新时间
    //~en Set fixed update time
    static void set_fixed_update_time(float time){fixed_update_time_ = time;}

private:
    static std::chrono::system_clock::time_point startup_time_;
    static float last_frame_time_;
    //~zh 上一帧花费的时间
    //~en The time spent on the last frame
    static float delta_time_;

    //~zh 固定更新时间，一般用于物理模拟
    //~en Fixed update time, usually used for physics simulation
    static float fixed_update_time_;
};


#endif //UNTITLED_TIME_H
