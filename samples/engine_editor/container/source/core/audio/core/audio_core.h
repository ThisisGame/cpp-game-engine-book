//
// Created by captain on 2021/8/1.
//

#ifndef UNTITLED_AUDIO_CORE_H
#define UNTITLED_AUDIO_CORE_H
#include "fmod/api/core/inc/fmod.h"

class AudioCore {
public:
    /// 初始化fmod
    static void Init();
    /// 创建音效
    /// \param name_or_data
    /// \param mode
    /// \param ex_info
    /// \param sound
    /// \return
    static FMOD_RESULT CreateSound(const char *name_or_data,FMOD_MODE mode,FMOD_CREATESOUNDEXINFO *ex_info,FMOD_SOUND **sound);

    /// 播放sound对应的音效
    /// \param sound
    /// \param channel_group
    /// \param paused
    /// \param channel
    /// \return
    static FMOD_RESULT PlaySound(FMOD_SOUND *sound,FMOD_CHANNELGROUP *channel_group,bool paused,FMOD_CHANNEL **channel);

    /// 刷帧
    /// \return
    static FMOD_RESULT Update();

    /// 设置听者属性
    /// \param listener
    /// \param pos
    /// \param vel
    /// \param forward
    /// \param up
    /// \return
    static FMOD_RESULT Set3DListenerAttributes(int listener, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel, const FMOD_VECTOR *forward, const FMOD_VECTOR *up);
private:
    static FMOD_SYSTEM * fmod_system_;//fmod音效引擎系统
};


#endif //UNTITLED_AUDIO_CORE_H
