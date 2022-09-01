//
// Created by captainchen on 2022/8/30.
//

#include "render_buffer.h"
#include "render_device/gpu_resource_mapper.h"
#include "render_device/render_task_producer.h"

RenderBuffer::RenderBuffer():RenderTarget() {
}

RenderBuffer::~RenderBuffer() {

}

void RenderBuffer::Init(unsigned short width, unsigned short height) {
    RenderTarget::Init(width,height);
    //创建RBO任务
    render_buffer_object_handle_ = GPUResourceMapper::GenerateRBOHandle();
    RenderTaskProducer::ProduceRenderTaskCreateRBO(render_buffer_object_handle_,width_,height_);
}