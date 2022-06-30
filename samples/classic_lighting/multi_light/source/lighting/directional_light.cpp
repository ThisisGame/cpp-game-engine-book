//
// Created by captain on 4/28/2022.
//

#include "directional_light.h"
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
    registration::class_<DirectionalLight>("DirectionalLight")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

DirectionalLight::DirectionalLight():Light()
{

}

DirectionalLight::~DirectionalLight() {

}

void DirectionalLight::set_color(glm::vec3 color){
    Light::set_color(color);
    UniformBufferObjectManager::UpdateUniformBlockSubData3f("Light","color",color_);
};

void DirectionalLight::set_intensity(float intensity){
    Light::set_intensity(intensity);
    UniformBufferObjectManager::UpdateUniformBlockSubData1f("Light","intensity",intensity_);
};

void DirectionalLight::Update(){
    glm::vec3 rotation=game_object()->GetComponent<Transform>()->rotation();
    glm::mat4 eulerAngleYXZ = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
    glm::vec3 light_rotation=glm::vec3(eulerAngleYXZ * glm::vec4(0,0,-1,0));
    UniformBufferObjectManager::UpdateUniformBlockSubData3f("Light","u_light_dir",light_rotation);
}