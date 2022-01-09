//
// Created by captain on 2021/8/5.
//

#ifndef UNTITLED_AUDIOLISTENER_H
#define UNTITLED_AUDIOLISTENER_H

#include "AK/SoundEngine/Common/AkTypes.h"
#include "component/component.h"

class AudioListener:public Component {
public:
    AudioListener();

    virtual void Awake();
    virtual void Update();

private:
    AkGameObjectID audio_listener_object_id_;
};


#endif //UNTITLED_AUDIOLISTENER_H
