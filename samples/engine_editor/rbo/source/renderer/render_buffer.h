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

    /// 使用
    virtual void Bind() override;

    virtual void UnBind() override;

    unsigned int render_buffer_object_handle(){
        return render_buffer_object_handle_;
    }

    void BlitFrameBuffer(int src_x,int src_y,int src_width,int src_height,int dst_x,int dst_y,int dst_width,int dst_height);
private:
    unsigned int render_buffer_object_handle_;//关联的RBO Handle
};


#endif //RBO_RENDER_BUFFER_H
