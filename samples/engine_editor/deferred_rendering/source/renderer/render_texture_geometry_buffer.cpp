//
// Created by captainchen on 2022/8/19.
//

#include "render_texture_geometry_buffer.h"
#include "renderer/texture_2d.h"
#include "render_device/gpu_resource_mapper.h"
#include "render_device/render_task_producer.h"

RenderTextureGeometryBuffer::RenderTextureGeometryBuffer(): RenderTexture(), frag_position_texture_2d_(nullptr), frag_normal_texture_2d_(nullptr), frag_color_texture_2d_(nullptr) {
}

RenderTextureGeometryBuffer::~RenderTextureGeometryBuffer() {
    if(frame_buffer_object_handle_>0){
        RenderTaskProducer::ProduceRenderTaskDeleteFBO(frame_buffer_object_handle_);
    }
    //删除Texture2D
    if(frag_position_texture_2d_ != nullptr){
        delete frag_position_texture_2d_;
    }
    if(frag_normal_texture_2d_ != nullptr){
        delete frag_normal_texture_2d_;
    }
    if(frag_color_texture_2d_ != nullptr){
        delete frag_color_texture_2d_;
    }
}

void RenderTextureGeometryBuffer::Init(unsigned short width, unsigned short height) {
    width_=width;
    height_=height;
    frag_position_texture_2d_=Texture2D::Create(width_, height_, GL_RGBA, GL_RGB, GL_FLOAT, nullptr, 0);
    frag_normal_texture_2d_=Texture2D::Create(width_, height_, GL_RGBA, GL_RGB, GL_FLOAT, nullptr, 0);
    frag_color_texture_2d_=Texture2D::Create(width_, height_, GL_RGBA, GL_RGB, GL_FLOAT, nullptr, 0);
    //创建FBO任务
    frame_buffer_object_handle_ = GPUResourceMapper::GenerateFBOHandle();
    RenderTaskProducer::ProduceRenderTaskCreateGBuffer(frame_buffer_object_handle_, width_, height_, frag_position_texture_2d_->texture_handle(), frag_normal_texture_2d_->texture_handle(), frag_color_texture_2d_->texture_handle());
}
