//
// Created by captainchen on 2022/6/7.
//

#include "environment.h"
#include "core/component/game_object.h"
#include "core/renderer/mesh_renderer.h"
#include "core/renderer/material.h"
#include "core/render_device/gpu_resource_mapper.h"
#include "core/render_device/render_task_producer.h"
#include "core/render_device/uniform_buffer_object_manager.h"

Environment::Environment():
    ambient_color_(glm::vec3(1.0f, 1.0f, 1.0f)),ambient_color_intensity_(1.0f),uniform_buffer_object_handle_(0) {
}

/// 设置环境颜色
void Environment::set_ambient_color(const glm::vec3 &ambient_color) {
    ambient_color_ = ambient_color;
    UniformBufferObjectManager::UpdateUniformBlockSubData3f("u_ambient","data.color",ambient_color_);
}


/// 设置环境强度
void Environment::set_ambient_color_intensity(float ambient_color_intensity){
    ambient_color_intensity_ = ambient_color_intensity;
    UniformBufferObjectManager::UpdateUniformBlockSubData1f("u_ambient","data.intensity",ambient_color_intensity_);
}