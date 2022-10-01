//
// Created by captainchen on 2021/8/10.
//

#include "audio_studio.h"
#include "app/application.h"
#include "utils/debug.h"

FMOD_STUDIO_SYSTEM* AudioStudio::system_;

FMOD_RESULT AudioStudio::Init() {
    FMOD_RESULT result=FMOD_Studio_System_Create(&system_, FMOD_VERSION);
    if(result!=FMOD_OK){
        return result;
    }
    result=FMOD_Studio_System_Initialize(system_, 1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
    if(result!=FMOD_OK){
        return result;
    }
    return FMOD_OK;
}

FMOD_RESULT AudioStudio::Update() {
    if (system_== nullptr){
        return FMOD_ERR_INITIALIZATION;
    }
    return FMOD_Studio_System_Update(system_);
}

FMOD_RESULT AudioStudio::LoadBankFile(string file_name) {
    string bank_path=Application::data_path()+file_name;
    FMOD_STUDIO_BANK* bank= nullptr;
    FMOD_RESULT result= FMOD_Studio_System_LoadBankFile(system_, bank_path.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
    if(result!=FMOD_OK){
        DEBUG_LOG_ERROR("FMOD_Studio_System_LoadBankFile result:{},file_name:{}",result,file_name);
    }
    return result;
}



AudioStudioEvent* AudioStudio::CreateEventInstance(const char *event_path) {
    FMOD_STUDIO_EVENTDESCRIPTION* event_description= nullptr;
    FMOD_RESULT result= FMOD_Studio_System_GetEvent(system_, event_path, &event_description);
    if(result!=FMOD_OK){
        DEBUG_LOG_ERROR("FMOD_Studio_System_GetEvent result:{},event_path:{}",result,event_path);
        return nullptr;
    }
    FMOD_STUDIO_EVENTINSTANCE* event_instance=nullptr;
    result= FMOD_Studio_EventDescription_CreateInstance(event_description,&event_instance);
    if(result!=FMOD_OK){
        DEBUG_LOG_ERROR("FMOD_Studio_EventDescription_CreateInstance result:{},event_path:{}",result,event_path);
        return nullptr;
    }
    AudioStudioEvent* audio_studio_event=new AudioStudioEvent();
    audio_studio_event->set_event_instance(event_instance);
    return audio_studio_event;
}

void AudioStudio::SetListenerAttributes(float x, float y, float z) {
    FMOD_3D_ATTRIBUTES attributes = { { x,y,z } };
    attributes.forward.z = 1.0f;
    attributes.up.y = 1.0f;
    FMOD_Studio_System_SetListenerAttributes(system_,0,&attributes,0);
}




