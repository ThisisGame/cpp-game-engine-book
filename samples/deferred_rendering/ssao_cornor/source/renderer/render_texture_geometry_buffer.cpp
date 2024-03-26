//
// Created by captainchen on 2022/8/19.
//

#include "render_texture_geometry_buffer.h"
#include "renderer/texture_2d.h"
#include "render_device/gpu_resource_mapper.h"
#include "render_device/render_task_producer.h"

RenderTextureGeometryBuffer::RenderTextureGeometryBuffer(): RenderTexture(), frag_position_texture_2d_(nullptr),
frag_normal_texture_2d_(nullptr),frag_vertex_color_texture_2d_(nullptr),frag_diffuse_color_texture_2d_(nullptr),
frag_specular_intensity_texture_2d_(nullptr),frag_specular_highlight_shininess_texture_2d_(nullptr){
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
    if(frag_vertex_color_texture_2d_ != nullptr){
        delete frag_vertex_color_texture_2d_;
    }
    if(frag_diffuse_color_texture_2d_ != nullptr){
        delete frag_diffuse_color_texture_2d_;
    }
    if(frag_specular_intensity_texture_2d_ != nullptr){
        delete frag_specular_intensity_texture_2d_;
    }
    if(frag_specular_highlight_shininess_texture_2d_ != nullptr){
        delete frag_specular_highlight_shininess_texture_2d_;
    }
}

void RenderTextureGeometryBuffer::Init(unsigned short width, unsigned short height) {
    width_=width;
    height_=height;
    //如果要在纹理中存储超过1的值，需要使用浮点纹理
    frag_position_texture_2d_=Texture2D::Create(width_, height_, GL_RGBA16F, GL_RGB,GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE, GL_FLOAT, nullptr, 0);
    frag_normal_texture_2d_=Texture2D::Create(width_, height_, GL_RGBA16F, GL_RGB, GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE, GL_FLOAT, nullptr, 0);
    frag_vertex_color_texture_2d_=Texture2D::Create(width_, height_, GL_RGBA, GL_RGB, GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE, GL_FLOAT, nullptr, 0);
    frag_diffuse_color_texture_2d_=Texture2D::Create(width_, height_, GL_RGBA, GL_RGB, GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE, GL_FLOAT, nullptr, 0);
    frag_specular_intensity_texture_2d_=Texture2D::Create(width_, height_, GL_RGBA, GL_RGB, GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE, GL_FLOAT, nullptr, 0);
    frag_specular_highlight_shininess_texture_2d_=Texture2D::Create(width_, height_, GL_RGBA16F, GL_RGB, GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE, GL_FLOAT, nullptr, 0);
    depth_texture_2d_=Texture2D::Create(width_,height_,GL_DEPTH_COMPONENT,GL_DEPTH_COMPONENT,GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE, GL_UNSIGNED_SHORT, nullptr,0);
    //创建FBO任务
    frame_buffer_object_handle_ = GPUResourceMapper::GenerateFBOHandle();
    RenderTaskProducer::ProduceRenderTaskCreateGBuffer(frame_buffer_object_handle_, width_, height_,
                                                       frag_position_texture_2d_->texture_handle(),
                                                       frag_normal_texture_2d_->texture_handle(),
                                                       frag_vertex_color_texture_2d_->texture_handle(),
                                                       frag_diffuse_color_texture_2d_->texture_handle(),
                                                       frag_specular_intensity_texture_2d_->texture_handle(),
                                                       frag_specular_highlight_shininess_texture_2d_->texture_handle(),
                                                       depth_texture_2d_->texture_handle());
}
