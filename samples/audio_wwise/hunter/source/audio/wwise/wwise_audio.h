//
// Created by captainchen on 2021/12/27.
//

#ifndef UNTITLED_WWISE_AUDIO_H
#define UNTITLED_WWISE_AUDIO_H

#include <glm/glm.hpp>
#include <AK/SoundEngine/Common/AkTypes.h>

class WwiseAudio {
public:
    static void Init();

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
    static void SetDefaultListeners(const AkGameObjectID& game_object_id);

    /// 设置物体位置
    static void SetPosition(AkGameObjectID game_object_id, glm::vec3 position,glm::vec3 front,glm::vec3 up);


private:
    static AkGameObjectID audio_object_id_next_;
};


#endif //UNTITLED_WWISE_AUDIO_H
