//
// Created by captainchen on 2022/8/30.
//

#include "render_buffer.h"
#include "render_device/gpu_resource_mapper.h"
#include "render_device/render_task_producer.h"

RenderBuffer::RenderBuffer():RenderTarget(RenderTargetType::RenderTargetBuffer) {
}

RenderBuffer::~RenderBuffer() {
    if(render_buffer_object_handle_>0){
        RenderTaskProducer::ProduceRenderTaskDeleteRBO(render_buffer_object_handle_);
    }
}

void RenderBuffer::Init(unsigned short width, unsigned short height) {
    RenderTarget::Init(width,height);
    //创建RBO任务
    render_buffer_object_handle_ = GPUResourceMapper::GenerateRBOHandle();
    RenderTaskProducer::ProduceRenderTaskCreateRBO(render_buffer_object_handle_,width_,height_);
}

void RenderBuffer::Bind(){
    RenderTaskProducer::ProduceRenderTaskFBOAttachRBO(frame_buffer_object_handle_,render_buffer_object_handle_);
    RenderTarget::Bind();
}

void RenderBuffer::UnBind(){
    BlitFrameBuffer(0,0,960,640,100,100,960,640);
    RenderTarget::UnBind();
}

void RenderBuffer::BlitFrameBuffer(int src_x,int src_y,int src_width,int src_height,int dst_x,int dst_y,int dst_width,int dst_height){
    RenderTaskProducer::ProduceRenderTaskBlitFrameBuffer(frame_buffer_object_handle_,0,src_x,
                                                         src_y,src_width,src_height,dst_x,dst_y,dst_width,dst_height,GL_COLOR_BUFFER_BIT,GL_NEAREST);
}