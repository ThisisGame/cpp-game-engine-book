//
// Created by captain on 2021/8/3.
//

#include "audio_source.h"
#include <rttr/registration>
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine
#include "component/game_object.h"
#include "component/transform.h"
#include "utils/debug.h"


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
    WwiseAudio::CreateAudioObject(audio_object_id_,"");
}

void AudioSource::SetEvent(const std::string &event_name) {
    event_name_=event_name;
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
    AkPlayingID playing_id = AK::SoundEngine::PostEvent(event_name_.c_str(), audio_object_id_ );
    if(playing_id==AK_INVALID_PLAYING_ID){
        DEBUG_LOG_ERROR("AudioSource::Play() failed");
    }
}

void AudioSource::Pause() {

}

void AudioSource::Stop() {

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

}

AudioSource::~AudioSource() {

}
