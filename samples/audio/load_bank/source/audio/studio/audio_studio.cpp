//
// Created by captainchen on 2021/8/10.
//

#include "audio_studio.h"
#include "utils/application.h"

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

FMOD_RESULT
AudioStudio::LoadBankFile(string file_name) {
    string bank_path=Application::data_path()+file_name;
    FMOD_STUDIO_BANK* bank= nullptr;
    return FMOD_Studio_System_LoadBankFile(system_, bank_path.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
}

FMOD_RESULT AudioStudio::SetParameterByName(const char *event_path,const char *name, float value, bool ignore_seek_speed) {
    FMOD_STUDIO_EVENTINSTANCE* event_instance= nullptr;
    FMOD_RESULT result=GetEventInstance(event_path,&event_instance);
    if(result!=FMOD_OK){
        return result;
    }
    return FMOD_Studio_EventInstance_SetParameterByName(event_instance,name,value,ignore_seek_speed);
}

FMOD_RESULT AudioStudio::GetEventInstance(const char *event_path,FMOD_STUDIO_EVENTINSTANCE** event_instance) {
    FMOD_STUDIO_EVENTDESCRIPTION* event_description= nullptr;
    FMOD_RESULT result= FMOD_Studio_System_GetEvent(system_, event_path, &event_description);
    if(event_description== nullptr){
        return result;
    }

    result= FMOD_Studio_EventDescription_CreateInstance(event_description,event_instance);
    return result;
}

FMOD_RESULT AudioStudio::SetEventInstance3DAttribute(const char *event_path, float x, float y, float z) {
    FMOD_STUDIO_EVENTINSTANCE* event_instance= nullptr;
    FMOD_RESULT result=GetEventInstance(event_path,&event_instance);
    if(result!=FMOD_OK){
        return result;
    }
    FMOD_3D_ATTRIBUTES attributes = { { x,y,z } };
    return FMOD_Studio_EventInstance_Set3DAttributes(event_instance,&attributes);
}


