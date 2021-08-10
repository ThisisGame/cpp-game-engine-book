//
// Created by captainchen on 2021/8/10.
//

#include "audio_studio.h"
#include "audio_studio_event.h"

FMOD_RESULT AudioStudio::Create(FMOD_STUDIO_SYSTEM **system, unsigned int header_version) {
    return FMOD_Studio_System_Create(system, header_version);
}

FMOD_RESULT AudioStudio::Initialize(int max_channels, FMOD_STUDIO_INITFLAGS fmod_studio_init_flags, FMOD_INITFLAGS fmod_init_flags,
                                    void *extra_driver_data) {
    return FMOD_Studio_System_Initialize(system_, max_channels, fmod_studio_init_flags, fmod_init_flags, extra_driver_data);
}

FMOD_RESULT AudioStudio::Update() {
    if (system_== nullptr){
        return FMOD_ERR_INITIALIZATION;
    }
    return FMOD_Studio_System_Update(system_);
}

FMOD_RESULT
AudioStudio::LoadBankFile(const char *file_name, FMOD_STUDIO_LOAD_BANK_FLAGS fmod_studio_load_bank_flags, FMOD_STUDIO_BANK **bank) {
    return FMOD_Studio_System_LoadBankFile(system_, file_name, fmod_studio_load_bank_flags, bank);
}

FMOD_RESULT AudioStudio::CreateEvent(const char *path) {
    FMOD_STUDIO_EVENTDESCRIPTION* event_description= nullptr;
    FMOD_RESULT result= FMOD_Studio_System_GetEvent(system_,path,&event_description);
    if(event_description== nullptr){
        return result;
    }

    FMOD_STUDIO_EVENTINSTANCE * event_instance= nullptr;
    result= FMOD_Studio_EventDescription_CreateInstance(event_description,&event_instance);
    if(event_instance== nullptr){
        return result;
    }

    AudioStudioEvent* audio_studio_event=new AudioStudioEvent();
    audio_studio_event->set_event_instance(event_instance);
}


