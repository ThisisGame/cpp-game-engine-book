//
// Created by captainchen on 2022/6/7.
//

#include "environment.h"
#include "component/game_object.h"
#include "renderer/mesh_renderer.h"
#include "renderer/material.h"
#include "render_device/gpu_resource_mapper.h"
#include "render_device/render_task_producer.h"
#include "render_device/uniform_buffer_object_manager.h"

Environment::Environment():
    ambient_color_(glm::vec3(1.0f, 1.0f, 1.0f)),ambient_color_intensity_(1.0f),uniform_buffer_object_handle_(0) {
}

/// 设置环境颜色
void Environment::set_ambient_color(const glm::vec3 &ambient_color) {
    ambient_color_ = ambient_color;
    UniformBufferObjectManager::UpdateUniformBlockSubData3f("u_ambient_light","color",ambient_color_);
}


/// 设置环境强度
void Environment::set_ambient_color_intensity(float ambient_color_intensity){
    ambient_color_intensity_ = ambient_color_intensity;
    UniformBufferObjectManager::UpdateUniformBlockSubData1f("u_ambient_light","intensity",ambient_color_intensity_);
}