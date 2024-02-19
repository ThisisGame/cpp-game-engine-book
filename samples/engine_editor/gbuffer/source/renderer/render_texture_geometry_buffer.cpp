//
// Created by captainchen on 2022/8/19.
//

#include "render_texture_geometry_buffer.h"
#include "renderer/texture_2d.h"
#include "render_device/gpu_resource_mapper.h"
#include "render_device/render_task_producer.h"

RenderTextureGeometryBuffer::RenderTextureGeometryBuffer():RenderTexture(),vertex_position_texture_2d_(nullptr), vertex_normal_texture_2d_(nullptr),vertex_color_texture_2d_(nullptr) {
}

RenderTextureGeometryBuffer::~RenderTextureGeometryBuffer() {
    if(frame_buffer_object_handle_>0){
        RenderTaskProducer::ProduceRenderTaskDeleteFBO(frame_buffer_object_handle_);
    }
    //删除Texture2D
    if(vertex_position_texture_2d_!= nullptr){
        delete vertex_position_texture_2d_;
    }
    if(vertex_normal_texture_2d_!= nullptr){
        delete vertex_normal_texture_2d_;
    }
    if(vertex_color_texture_2d_!= nullptr){
        delete vertex_color_texture_2d_;
    }
}

void RenderTextureGeometryBuffer::Init(unsigned short width, unsigned short height) {
    width_=width;
    height_=height;
    vertex_position_texture_2d_=Texture2D::Create(width_,height_,GL_RGB,GL_RGB,GL_UNSIGNED_SHORT_5_6_5, nullptr,0);
    vertex_normal_texture_2d_=Texture2D::Create(width_,height_,GL_RGB,GL_RGB,GL_UNSIGNED_SHORT_5_6_5, nullptr,0);
    vertex_color_texture_2d_=Texture2D::Create(width_,height_,GL_RGB,GL_RGB,GL_UNSIGNED_SHORT_5_6_5, nullptr,0);
    //创建FBO任务
    frame_buffer_object_handle_ = GPUResourceMapper::GenerateFBOHandle();
    RenderTaskProducer::ProduceRenderTaskCreateGBuffer(frame_buffer_object_handle_,width_,height_,vertex_position_texture_2d_->texture_handle(),vertex_color_texture_2d_->texture_handle(),vertex_normal_texture_2d_->texture_handle());
}
