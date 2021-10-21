//
// Created by captain on 2021/10/18.
//

#ifndef UNTITLED_RENDER_DEVICE_OPENGL_H
#define UNTITLED_RENDER_DEVICE_OPENGL_H

#include "render_device.h"
class RenderDeviceOpenGL: public RenderDevice{
public:
    void Enable(RenderState render_state) override;

    void Disable(RenderState render_state) override;
};


#endif //UNTITLED_RENDER_DEVICE_OPENGL_H
