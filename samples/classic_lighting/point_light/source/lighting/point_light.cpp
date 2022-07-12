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

PointLight::PointLight():Light(),attenuation_constant_(0),attenuation_linear_(0),attenuation_quadratic_(0)
{

}

PointLight::~PointLight() {

}

void PointLight::set_attenuation_constant(float attenuation_constant){
    attenuation_constant_ = attenuation_constant;
    UniformBufferObjectManager::UpdateUniformBlockSubData1f("PointLight","u_light_constant",attenuation_constant_);
}

void PointLight::set_attenuation_linear(float attenuation_linear){
    attenuation_linear_ = attenuation_linear;
    UniformBufferObjectManager::UpdateUniformBlockSubData1f("PointLight","u_light_linear",attenuation_linear_);
}

void PointLight::set_attenuation_quadratic(float attenuation_quadratic){
    attenuation_quadratic_ = attenuation_quadratic;
    UniformBufferObjectManager::UpdateUniformBlockSubData1f("PointLight","u_light_quadratic",attenuation_quadratic_);
}

void PointLight::Update(){
    glm::vec3 light_position=game_object()->GetComponent<Transform>()->position();
    UniformBufferObjectManager::UpdateUniformBlockSubData3f("PointLight","u_light_pos",light_position);
}