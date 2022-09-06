//
// Created by captainchen on 2022/8/30.
//

#ifndef RBO_RENDER_BUFFER_H
#define RBO_RENDER_BUFFER_H

#include "render_target.h"

class RenderBuffer:public RenderTarget {
public:
    RenderBuffer();
    ~RenderBuffer();

    virtual void Init(unsigned short width,unsigned short height) override;

    unsigned int render_buffer_object_handle(){
        return render_buffer_object_handle_;
    }
private:
    unsigned int render_buffer_object_handle_;//关联的RBO Handle
};


#endif //RBO_RENDER_BUFFER_H
