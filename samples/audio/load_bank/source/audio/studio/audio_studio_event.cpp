//
// Created by captainchen on 2021/8/10.
//

#include "audio_studio_event.h"
#include "spdlog/spdlog.h"
#include "component/game_object.h"
#include "component/transform.h"

AudioStudioEvent::AudioStudioEvent() {}
AudioStudioEvent::~AudioStudioEvent(){}


FMOD_RESULT AudioStudioEvent::SetParameterByName(const char *name, float value, bool ignore_seek_speed) {
    return FMOD_Studio_EventInstance_SetParameterByName(event_instance_,name,value,ignore_seek_speed);
}

FMOD_RESULT AudioStudioEvent::Set3DAttribute(float x,float y,float z){
    FMOD_3D_ATTRIBUTES attributes = { { x,y,z } };
    return FMOD_Studio_EventInstance_Set3DAttributes(event_instance_,&attributes);
}