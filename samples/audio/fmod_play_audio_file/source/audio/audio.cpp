//
// Created by captain on 2021/8/1.
//

#include "audio.h"
#include "spdlog/spdlog.h"

FMOD_SYSTEM* Audio::fmod_system_;

void Audio::InitFmod(){
    FMOD_RESULT       result;
    unsigned int      version;

    /*
        Create a System object and initialize
    */
    result=FMOD_System_Create(&fmod_system_);
    spdlog::critical("FMOD::System_Create ret code {}",result);

    result = FMOD_System_GetVersion(fmod_system_,&version);
    spdlog::critical("FMOD_System_GetVersion ret code {}",result);

    if (version < FMOD_VERSION)
    {
        spdlog::critical("FMOD lib version {} doesn't match header version {}", version, FMOD_VERSION);
        return;
    }

    result=FMOD_System_Init(fmod_system_,32,FMOD_INIT_NORMAL,0);

    spdlog::critical("FMOD_System_Init ret code {}",result);
}


FMOD_RESULT Audio::Update() {
    return FMOD_System_Update(fmod_system_);
}

FMOD_RESULT
Audio::CreateSound(const char *name_or_data,FMOD_MODE mode,FMOD_CREATESOUNDEXINFO *ex_info,FMOD_SOUND **sound) {
    return FMOD_System_CreateSound(fmod_system_,name_or_data,mode,ex_info,sound);
}


FMOD_RESULT Audio::PlaySound(FMOD_SOUND *sound,FMOD_CHANNELGROUP *channel_group,bool paused,FMOD_CHANNEL **channel) {
    return FMOD_System_PlaySound(fmod_system_,sound,channel_group,paused,channel);
}
