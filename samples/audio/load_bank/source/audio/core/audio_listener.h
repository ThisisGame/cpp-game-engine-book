//
// Created by captain on 2021/8/5.
//

#ifndef UNTITLED_AUDIOLISTENER_H
#define UNTITLED_AUDIOLISTENER_H

#include "component/component.h"
#include "audio_clip.h"

class AudioListener:public Component {
public:
    AudioListener();

    virtual void Awake();
    virtual void Update();

private:
    unsigned int listener_id_=0;
    static unsigned int listener_num_;//已经创建的listener
};


#endif //UNTITLED_AUDIOLISTENER_H
