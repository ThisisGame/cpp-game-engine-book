//
// Created by captainchen on 2021/8/10.
//

#include "audio_studio_event.h"
#include <rttr/registration>
#include "spdlog/spdlog.h"
#include "component/game_object.h"
#include "component/transform.h"

using namespace rttr;
RTTR_REGISTRATION
{
    registration::class_<AudioStudioEvent>("AudioStudioEvent")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

AudioStudioEvent::AudioStudioEvent():Component() {}
AudioStudioEvent::~AudioStudioEvent(){
    if(event_instance_!= nullptr){
        delete event_instance_;
    }
}


FMOD_RESULT AudioStudioEvent::SetParameterByName(const char *name, float value, bool ignore_seek_speed) {
    return FMOD_Studio_EventInstance_SetParameterByName(event_instance_,name,value,ignore_seek_speed);
}

void AudioStudioEvent::Update() {
    Component::Update();
    auto component_transform=game_object()->GetComponent("Transform");
    auto transform=dynamic_cast<Transform*>(component_transform);
    if(!transform){
        return;
    }
    auto pos=transform->position();
    FMOD_3D_ATTRIBUTES attributes = { { pos.x,pos.y,pos.z } };
    FMOD_Studio_EventInstance_Set3DAttributes(event_instance_,&attributes);
}