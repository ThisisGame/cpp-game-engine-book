//
// Created by captainchen on 2022/8/19.
//

#include "noise_texture.h"
#include "renderer/texture_2d.h"
#include "render_device/gpu_resource_mapper.h"
#include "render_device/render_task_producer.h"

NoiseTexture::NoiseTexture(): width_(128), height_(128),in_use_(false),
                              noise_texture_2d_(nullptr) {
}

NoiseTexture::~NoiseTexture() {
    //删除Texture2D
    if(noise_texture_2d_ != nullptr){
        delete noise_texture_2d_;
    }
}

void NoiseTexture::Init(unsigned short width, unsigned short height,std::vector<glm::vec3> noise) {
    width_=width;
    height_=height;
    noise_texture_2d_=Texture2D::Create(width_, height_, GL_RGBA16F, GL_RGB,
                                        GL_LINEAR,GL_LINEAR,
                                        GL_REPEAT,GL_REPEAT,
                                        GL_FLOAT, (unsigned char*)&noise[0],noise.size()*sizeof(glm::vec3));
}
