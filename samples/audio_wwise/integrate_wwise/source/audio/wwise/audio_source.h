//
// Created by captain on 2021/8/3.
//

#ifndef UNTITLED_AUDIOSOURCE_H
#define UNTITLED_AUDIOSOURCE_H
#include <string>
#include "component/component.h"
#include "wwise_audio.h"

class AudioSource:public Component {
public:
    AudioSource();
    ~AudioSource();

    void SetEvent(const std::string &event_name);

    /// 设置为3D/2D音乐
    /// \param mode_3d
    void Set3DMode(bool mode_3d);

    /// 设置是否循环
    /// \param mode_loop
    void SetLoop(bool mode_loop);

    void SetRTPCValue(const std::string &rtpc_name, float value);

    void Play();
    void Pause();
    void Stop();

    bool Paused();

private:
    void Awake() override;
    void Update() override;

private:
    AkGameObjectID audio_object_id_;
    std::string event_name_;
    bool mode_3d_;
    bool mode_loop_;
    bool paused_;
};


#endif //UNTITLED_AUDIOSOURCE_H
