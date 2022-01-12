//
// Created by captainchen on 2021/12/27.
//

#ifndef UNTITLED_WWISE_AUDIO_H
#define UNTITLED_WWISE_AUDIO_H

#include <glm/glm.hpp>
#include <AK/SoundEngine/Common/AkTypes.h>
#include "AK/SoundEngine/Common/AkCallback.h"

class WwiseAudio {
public:
    static void Init();

    /// 刷帧
    static void Update();

    /// 加载 bank文件
    /// @param bank_name bank文件名
    static void LoadBank(const char *bank_name);

    /// 创建声音对象，声音需要附着在对象上。
    /// @param audio_object_id 对象id
    /// @param audio_object_name 对象名，用来profile显示
    static void CreateAudioObject(AkGameObjectID audio_object_id,const char *audio_object_name);

    /// 生成GameObjectID
    static AkGameObjectID GeneratorGameObjectID();

    /// 设置默认Listener
    /// @paran game_object_id Listener所在的Wwise GameObjectID
    static void SetDefaultListeners(const AkGameObjectID& game_object_id);

    /// 设置物体位置
    /// @param game_object_id 物体id
    /// @param position 位置
    /// @param front 前方
    /// @param up 上方
    static void SetPosition(AkGameObjectID game_object_id, glm::vec3 position,glm::vec3 front,glm::vec3 up);

    /// 触发Event播放音效
    /// @param event_name Event名
    /// @param audio_object_id 音频物体id
    /// @param flags 哪些情况下需要回调
    /// @param callback 回调
    /// @param user_data 回调用户数据
    /// @return 播放id
    static AkPlayingID PostEvent(const char* event_name,AkGameObjectID audio_object_id,AkUInt32 flags = 0,AkCallbackFunc callback = NULL,void* user_data = NULL);

    /// 设置实时参数控制值
    /// @param realtime_parameter_control_name 实时参数控制名
    /// @param value 值
    /// @param audio_object_id 音频物体id
    static void SetRTPCValue(const char* realtime_parameter_control_name,AkRtpcValue value,AkGameObjectID audio_object_id);

    /// 停止Event播放音效
    /// @param playing_id 播放id
    static void StopEvent(AkPlayingID playing_id);
private:
    static AkGameObjectID audio_object_id_next_;//下一个id
};


#endif //UNTITLED_WWISE_AUDIO_H
