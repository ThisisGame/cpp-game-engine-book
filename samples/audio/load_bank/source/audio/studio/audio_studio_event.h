//
// Created by captainchen on 2021/8/10.
//

#ifndef UNTITLED_AUDIO_STUDIO_EVENT_H
#define UNTITLED_AUDIO_STUDIO_EVENT_H

#include "fmod/api/core/inc/fmod.h"
#include "fmod/api/studio/inc/fmod_studio.h"

class AudioStudioEvent {
public:
    AudioStudioEvent();
    ~AudioStudioEvent();

    FMOD_STUDIO_EVENTINSTANCE* event_instance(){return event_instance_;}
    void set_event_instance(FMOD_STUDIO_EVENTINSTANCE* event_instance){event_instance_=event_instance;}

    /// 设置Event参数
    /// \param name
    /// \param value
    /// \param ignore_seek_speed
    /// \return
    FMOD_RESULT SetParameterByName(const char *name, float value, bool ignore_seek_speed = false);

    /// 设置Event 3D属性
    /// \param x
    /// \param y
    /// \param z
    /// \return
    FMOD_RESULT Set3DAttribute(float x,float y,float z);
private:
    FMOD_STUDIO_EVENTINSTANCE* event_instance_;
};


#endif //UNTITLED_AUDIO_STUDIO_EVENT_H
