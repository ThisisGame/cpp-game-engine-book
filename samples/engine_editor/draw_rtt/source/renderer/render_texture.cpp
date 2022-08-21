//
// Created by captainchen on 2022/8/19.
//

#include "render_texture.h"
#include "renderer/texture_2d.h"
#include "render_device/gpu_resource_mapper.h"
#include "render_device/render_task_producer.h"

RenderTexture::RenderTexture(): width_(128), height_(128), frame_buffer_object_handle_(0),in_use_(false),
                                color_texture_2d_(nullptr),depth_texture_2d_(nullptr) {
}

RenderTexture::~RenderTexture() {
    if(frame_buffer_object_handle_>0){
        RenderTaskProducer::ProduceRenderTaskDeleteFBO(frame_buffer_object_handle_);
    }
    //删除Texture2D
    if(color_texture_2d_!= nullptr){
        delete color_texture_2d_;
    }
    if(depth_texture_2d_!= nullptr){
        delete depth_texture_2d_;
    }
}

void RenderTexture::Init(unsigned short width, unsigned short height) {
    width_=width;
    height_=height;
    color_texture_2d_=Texture2D::Create(width_,height_,GL_RGB,GL_RGB,GL_UNSIGNED_SHORT_5_6_5, nullptr,0);
    depth_texture_2d_=Texture2D::Create(width_,height_,GL_DEPTH_COMPONENT,GL_DEPTH_COMPONENT,GL_UNSIGNED_SHORT, nullptr,0);
    //创建FBO任务
    frame_buffer_object_handle_ = GPUResourceMapper::GenerateFBOHandle();
    RenderTaskProducer::ProduceRenderTaskCreateFBO(frame_buffer_object_handle_,width_,height_,color_texture_2d_->texture_handle(),depth_texture_2d_->texture_handle());
}
