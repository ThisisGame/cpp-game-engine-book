//
// Created by captain on 2021/8/3.
//

#include "audio_clip.h"
#include "core/app/application.h"
#include "core/utils/debug.h"

AudioClip::AudioClip()=default;

AudioClip::~AudioClip() {
    if(fmod_sound_!= nullptr){
        FMOD_Sound_Release(fmod_sound_);
        fmod_sound_= nullptr;
    }
}

AudioClip * AudioClip::LoadFromFile(std::string audio_file_path) {
    FMOD_SOUND* fmod_sound;
    FMOD_RESULT result = AudioCore::CreateSound((Application::data_path() + audio_file_path).c_str(), FMOD_DEFAULT,
                                                nullptr, &fmod_sound);
    if(result!=FMOD_OK){
        DEBUG_LOG_ERROR("AudioCore::CreateSound failed");
        return nullptr;
    }

    AudioClip* audio_clip=new AudioClip();
    audio_clip->fmod_sound_=fmod_sound;
    return audio_clip;
}