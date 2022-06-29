//
// Created by captain on 4/28/2022.
//

#include "point_light.h"
#include <rttr/registration>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "component/game_object.h"
#include "component/transform.h"
#include "renderer/mesh_renderer.h"
#include "renderer/material.h"
#include "render_device/uniform_buffer_object_manager.h"

using namespace rttr;
RTTR_REGISTRATION//注册反射
{
    registration::class_<PointLight>("PointLight")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

unsigned short PointLight::light_count_=0;

PointLight::PointLight():Light(),attenuation_constant_(0),attenuation_linear_(0),attenuation_quadratic_(0)
{
    light_id_=light_count_;
    light_count_++;
}

PointLight::~PointLight() {

}


void PointLight::set_color(glm::vec3 color){
    Light::set_color(color);
    std::string uniform_block_member_name=fmt::format("u_light_array[{}].u_light_color",light_id_);
    UniformBufferObjectManager::UpdateUniformBlockSubData3f("MultiLight",uniform_block_member_name,color_);
};

void PointLight::set_intensity(float intensity){
    Light::set_intensity(intensity);
    std::string uniform_block_member_name=fmt::format("u_light_array[{}].u_light_intensity",light_id_);
    UniformBufferObjectManager::UpdateUniformBlockSubData1f("MultiLight",uniform_block_member_name,intensity_);
};

void PointLight::set_attenuation_constant(float attenuation_constant){
    attenuation_constant_ = attenuation_constant;
    std::string uniform_block_member_name=fmt::format("u_light_array[{}].u_light_constant",light_id_);
    UniformBufferObjectManager::UpdateUniformBlockSubData1f("MultiLight",uniform_block_member_name,attenuation_constant_);
}

void PointLight::set_attenuation_linear(float attenuation_linear){
    attenuation_linear_ = attenuation_linear;
    std::string uniform_block_member_name=fmt::format("u_light_array[{}].u_light_linear",light_id_);
    UniformBufferObjectManager::UpdateUniformBlockSubData1f("MultiLight",uniform_block_member_name,attenuation_linear_);
}

void PointLight::set_attenuation_quadratic(float attenuation_quadratic){
    attenuation_quadratic_ = attenuation_quadratic;
    std::string uniform_block_member_name=fmt::format("u_light_array[{}].u_light_quadratic",light_id_);
    UniformBufferObjectManager::UpdateUniformBlockSubData1f("MultiLight",uniform_block_member_name,attenuation_quadratic_);
}

void PointLight::Update(){
    glm::vec3 light_position=game_object()->GetComponent<Transform>()->position();
    std::string uniform_block_member_name=fmt::format("u_light_array[{}].u_light_pos",light_id_);
    UniformBufferObjectManager::UpdateUniformBlockSubData3f("MultiLight",uniform_block_member_name,light_position);
}