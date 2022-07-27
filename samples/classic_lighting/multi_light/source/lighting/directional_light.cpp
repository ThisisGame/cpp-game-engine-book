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

unsigned short DirectionalLight::light_count_=0;

DirectionalLight::DirectionalLight():Light()
{
    light_id_=light_count_;
    light_count_++;
    UniformBufferObjectManager::UpdateUniformBlockSubData1f("u_directional_light_array","actually_used_count",light_count_);
}

DirectionalLight::~DirectionalLight() {

}

void DirectionalLight::set_color(glm::vec3 color){
    Light::set_color(color);
    std::string uniform_block_member_name=fmt::format("data[{}].color",light_id_);
    UniformBufferObjectManager::UpdateUniformBlockSubData3f("u_directional_light_array",uniform_block_member_name,color_);
};

void DirectionalLight::set_intensity(float intensity){
    Light::set_intensity(intensity);
    std::string uniform_block_member_name=fmt::format("data[{}].intensity",light_id_);
    UniformBufferObjectManager::UpdateUniformBlockSubData1f("u_directional_light_array",uniform_block_member_name,intensity_);
};

void DirectionalLight::Update(){
    glm::vec3 rotation=game_object()->GetComponent<Transform>()->rotation();
    glm::mat4 eulerAngleYXZ = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
    glm::vec3 light_rotation=glm::vec3(eulerAngleYXZ * glm::vec4(0,0,-1,0));
    std::string uniform_block_member_name=fmt::format("data[{}].dir",light_id_);
    UniformBufferObjectManager::UpdateUniformBlockSubData3f("u_directional_light_array",uniform_block_member_name,light_rotation);
}