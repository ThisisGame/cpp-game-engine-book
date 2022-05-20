//
// Created by captain on 2021/8/5.
//

#include "audio_listener.h"
#include <rttr/registration>
#include "component/game_object.h"
#include "component/transform.h"

using namespace rttr;
RTTR_REGISTRATION
{
registration::class_<AudioListener>("AudioListener")
.constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

unsigned int AudioListener::listener_num_=0;
AudioListener::AudioListener():Component() {
    listener_id_=listener_num_++;
}

void AudioListener::Awake() {}

void AudioListener::Update() {
    Component::Update();
    auto component_transform=game_object()->GetComponent("Transform");
    auto transform=dynamic_cast<Transform*>(component_transform);
    if(!transform){
        return;
    }
    auto pos=transform->position();

    FMOD_VECTOR vel = {  0.0f, 0.0f, 0.0f };
    FMOD_VECTOR audio_listener_pos = {  pos.x, pos.y, pos.z };
    FMOD_VECTOR forward = {  0.0f, 0.0f, 1.0f };
    FMOD_VECTOR up = {  0.0f, 1.0f, 0.0f };
    AudioCore::Set3DListenerAttributes(listener_id_, &audio_listener_pos, &vel, &forward, &up);
}