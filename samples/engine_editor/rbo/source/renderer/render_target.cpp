//
// Created by captainchen on 2022/8/30.
//

#include "render_target.h"
#include "render_device/render_task_producer.h"

RenderTarget::RenderTarget():width_(128), height_(128), frame_buffer_object_handle_(0),in_use_(false) {
}

RenderTarget::~RenderTarget() {
    if(frame_buffer_object_handle_>0){
        RenderTaskProducer::ProduceRenderTaskDeleteFBO(frame_buffer_object_handle_);
    }
}

void RenderTarget::Init(unsigned short width, unsigned short height) {
    width_=width;
    height_=height;
}