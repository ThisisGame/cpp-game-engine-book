//
// Created by captainchen on 2021/8/10.
//

#ifndef UNTITLED_AUDIO_STUDIO_H
#define UNTITLED_AUDIO_STUDIO_H

#include <iostream>
#include "fmod/api/core/inc/fmod.h"
#include "fmod/api/studio/inc/fmod_studio.h"

using namespace std;

class AudioStudio {
public:
    static FMOD_RESULT Init();

    /// 刷帧
    /// \return
    static FMOD_RESULT Update();

    /// 加载 bank 文件
    /// \param file_name
    /// \return
    static FMOD_RESULT LoadBankFile(string file_name);

    /// 设置Event参数
    /// \param name
    /// \param value
    /// \param ignore_seek_speed
    /// \return
    static FMOD_RESULT SetParameterByName(const char *event_path,const char *name, float value, bool ignore_seek_speed = false);

    /// 设置Event 3D属性
    /// \param event_path
    /// \param x
    /// \param y
    /// \param z
    /// \return
    static FMOD_RESULT SetEventInstance3DAttribute(const char *event_path,float x,float y,float z);

    /// 获取Event实例，如果没有就创建。
    /// \param event_path
    /// \return
    static FMOD_RESULT GetEventInstance(const char *event_path,FMOD_STUDIO_EVENTINSTANCE** event_instance);

private:
    static FMOD_STUDIO_SYSTEM* system_;
};


#endif //UNTITLED_AUDIO_STUDIO_H
