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

    /// 设置Event名
    void SetEvent(const std::string &event_name);

    /// 设置实时控制参数
    void SetRTPCValue(const std::string &realtime_parameter_control_name, float value);

    /// 播放
    void Play();

    /// 停止
    void Stop();

    /// 设置音效播放完成回调
    /// @param callback 回调函数，如果是lua传入，使用普通function就行
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

    std::function<void()> event_end_callback_;//AudioSource实例播放结束回调

    /********************  回调处理  ********************/
public:
    /// Event回调，静态函数
    /// \param in_eType Event类型。
    /// \param in_pCookie 回调参数，在PostEvent设置的，原样返回。
    static void MusicCallback(AkCallbackType in_eType,AkCallbackInfo* in_pCallbackInfo);

    /// 处理AudioSource回调队列
    static void ExecuteMusicCallbackQueue();
private:
    /// Event回调信息
    class MusicCallbackInfo{
    public:
        MusicCallbackInfo(AkCallbackType callback_type, AkCallbackInfo callback_info)
                :callback_type_(callback_type), callback_info_(callback_info){}
        ~MusicCallbackInfo(){}

        AkCallbackType callback_type_;//回调类型，停止、暂停、退出等事件枚举
        AkCallbackInfo callback_info_;//回调参数，包括音频对象ID、透传参数。
    };

    static rigtorp::SPSCQueue<MusicCallbackInfo> event_callback_queue_;//AudioSource回调队列，单线程写单线程读，线程安全。
};


#endif //UNTITLED_AUDIOSOURCE_H
