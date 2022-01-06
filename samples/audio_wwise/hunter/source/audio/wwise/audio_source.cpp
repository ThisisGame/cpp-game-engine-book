//
// Created by captain on 2021/8/3.
//

#include "audio_source.h"
#include <rttr/registration>
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine
#include "component/game_object.h"
#include "component/transform.h"
#include "utils/debug.h"

rigtorp::SPSCQueue<AudioSource::MusicCallbackInfo> AudioSource::event_callback_queue_(1024);

using namespace rttr;
RTTR_REGISTRATION
{
registration::class_<AudioSource>("AudioSource")
.constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

AudioSource::AudioSource():Component() {

}

void AudioSource::Awake() {
    audio_object_id_=WwiseAudio::GeneratorGameObjectID();
    WwiseAudio::CreateAudioObject(audio_object_id_,game_object()->name().c_str());
}

void AudioSource::SetEvent(const std::string &event_name) {
    event_name_=event_name;
}


void AudioSource::MusicCallback(AkCallbackType in_eType,AkCallbackInfo* in_pCallbackInfo)
{
    MusicCallbackInfo music_callback_info(in_eType,*in_pCallbackInfo);
    event_callback_queue_.push(music_callback_info);
}

void AudioSource::SetRTPCValue(const std::string &rtpc_name, float value) {
    AKRESULT result = AK::SoundEngine::SetRTPCValue(rtpc_name.c_str(), value, audio_object_id_);
    if (result != AK_Success) {
        DEBUG_LOG_ERROR("Set RTPC value failed,rtpc_name:{} ,value:{}", rtpc_name, value);
    }
}

void AudioSource::Set3DMode(bool mode_3d) {
    if(mode_3d){

    }else{

    }

}

void AudioSource::Play() {
    playing_id_ = AK::SoundEngine::PostEvent(event_name_.c_str(),audio_object_id_,AK_EndOfEvent,&AudioSource::MusicCallback,this);
    if(playing_id_==AK_INVALID_PLAYING_ID){
        DEBUG_LOG_ERROR("AudioSource::Play() failed");
        return;
    }
}

void AudioSource::Pause() {

}

void AudioSource::Stop() {
    AK::SoundEngine::ExecuteActionOnPlayingID(AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Stop,playing_id_);
}

bool AudioSource::Paused() {
    return false;
}

void AudioSource::SetLoop(bool mode_loop) {
    if(mode_loop){

    }else{

    }

}

void AudioSource::Update() {
    Component::Update();

    // 处理Event
    while (event_callback_queue_.front()){
        MusicCallbackInfo* music_callback_info = event_callback_queue_.front();

        AudioSource* audio_source=(AudioSource*)music_callback_info->callback_info_.pCookie;
        if(music_callback_info->callback_type_ == AK_EndOfEvent){
            DEBUG_LOG_INFO("end event: {}",audio_source->event_name_);
            if(audio_source->event_end_callback_){
                audio_source->event_end_callback_();
            }
        }
        event_callback_queue_.pop();
    }

    auto component_transform=game_object()->GetComponent("Transform");
    auto transform=dynamic_cast<Transform*>(component_transform);
    if(!transform){
        return;
    }
    auto pos=transform->position();

    WwiseAudio::SetPosition(audio_object_id_,pos,glm::vec3(0,0,1),glm::vec3(0,1,0));
}

AudioSource::~AudioSource() {

}
