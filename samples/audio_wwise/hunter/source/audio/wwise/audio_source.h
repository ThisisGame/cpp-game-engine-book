//
// Created by captain on 2021/8/3.
//

#ifndef UNTITLED_AUDIOSOURCE_H
#define UNTITLED_AUDIOSOURCE_H
#include <string>
#include <functional>
#include <AK/SoundEngine/Common/AkCallback.h>
#include <spscqueue/include/rigtorp/SPSCQueue.h>
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

    /// Event回调信息
    class MusicCallbackInfo{
    public:
        MusicCallbackInfo(AkCallbackType callback_type, AkCallbackInfo callback_info)
                :callback_type_(callback_type), callback_info_(callback_info){}
        ~MusicCallbackInfo(){}

        AkCallbackType callback_type_;
        AkCallbackInfo callback_info_;
    };

    /// Event回调，静态函数
    /// \param in_eType Event类型。
    /// \param in_pCookie 回调参数，在PostEvent设置的，原样返回。
    static void MusicCallback(AkCallbackType in_eType,AkCallbackInfo* in_pCallbackInfo);

    void set_event_end_callback(std::function<void(void)> callback){
        event_end_callback_ = callback;
    }

private:
    void Awake() override;
    void Update() override;

private:
    AkGameObjectID audio_object_id_;
    std::string event_name_;
    AkPlayingID playing_id_;
    bool mode_3d_;
    bool mode_loop_;
    bool paused_;

    std::function<void()> event_end_callback_;

    static rigtorp::SPSCQueue<MusicCallbackInfo> event_callback_queue_;
};


#endif //UNTITLED_AUDIOSOURCE_H
