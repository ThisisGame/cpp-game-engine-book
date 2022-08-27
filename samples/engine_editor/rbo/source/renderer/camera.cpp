//
// Created by captain on 2021/5/25.
//

#include "camera.h"
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glad/gl.h>
#include <rttr/registration>
#include "render_texture.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "render_device/render_task_producer.h"
#include "render_device/gpu_resource_mapper.h"


using namespace rttr;
RTTR_REGISTRATION//注册反射
{
    registration::class_<Camera>("Camera")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

std::vector<Camera*> Camera::all_camera_;
Camera* Camera::current_camera_;

Camera::Camera():Component(),clear_color_(49.f/255,77.f/255,121.f/255,1.f),
    clear_flag_(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT),depth_(0),culling_mask_(0x01),
    target_render_texture_(nullptr) {
    //默认获取现有Camera最大depth，设置当前Camera.depth +1
    if (all_camera_.size()>0){
        unsigned char max_depth=all_camera_.back()->depth();
        depth_=max_depth+1;
    }
    all_camera_.push_back(this);
}


Camera::~Camera() {
    auto iter=std::find(all_camera_.begin(),all_camera_.end(),this);
    if(iter!=all_camera_.end()){
        all_camera_.erase(iter);
    }
}

void Camera::SetView(const glm::vec3 &cameraForward,const glm::vec3 &cameraUp) {
    auto transform=game_object()->GetComponent<Transform>();
    view_mat4_=glm::lookAt(transform->position(), cameraForward, cameraUp);

    glm::vec3 rotation=transform->rotation();
    glm::mat4 eulerAngleYXZ = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));

    view_mat4_=eulerAngleYXZ*view_mat4_;
}

void Camera::SetPerspective(float fovDegrees, float aspectRatio, float nearClip, float farClip) {
    projection_mat4_=glm::perspective(glm::radians(fovDegrees),aspectRatio,nearClip,farClip);
}

void Camera::SetOrthographic(float left,float right,float bottom,float top,float z_near,float z_far) {
    projection_mat4_=glm::ortho(left,right,bottom,top,z_near,z_far);
}

void Camera::Clear() {
    RenderTaskProducer::ProduceRenderTaskSetClearFlagAndClearColorBuffer(clear_flag_, clear_color_.r, clear_color_.g,
                                                                         clear_color_.b, clear_color_.a);
}

void Camera::CheckRenderToTexture(){
    if(target_render_texture_== nullptr){//没有设置目标RenderTexture
        return;
    }
    if(target_render_texture_->in_use()){
        return;
    }
    if(target_render_texture_->frame_buffer_object_handle() == 0){//还没有初始化，没有生成FBO。
        return;
    }
    RenderTaskProducer::ProduceRenderTaskBindFBO(target_render_texture_->frame_buffer_object_handle());
    target_render_texture_->set_in_use(true);
}

void Camera::CheckCancelRenderToTexture(){
    if(target_render_texture_== nullptr){//没有设置目标RenderTexture
        return;
    }
    if(target_render_texture_->in_use()==false){
        return;
    }
    if(target_render_texture_->frame_buffer_object_handle() == 0){//还没有初始化，没有生成FBO。
        return;
    }
    RenderTaskProducer::ProduceRenderTaskUnBindFBO(target_render_texture_->frame_buffer_object_handle());
    target_render_texture_->set_in_use(false);
}

void Camera::set_target_render_texture(RenderTexture* render_texture){
    if(render_texture== nullptr){
        clear_target_render_texture();
    }
    target_render_texture_=render_texture;
}

void Camera::clear_target_render_texture() {
    if(target_render_texture_== nullptr){//没有设置目标RenderTexture
        return;
    }
    if(target_render_texture_->in_use()== false){
        return;
    }
    RenderTaskProducer::ProduceRenderTaskUnBindFBO(target_render_texture_->frame_buffer_object_handle());
    target_render_texture_->set_in_use(false);
}

void Camera::set_depth(unsigned char depth) {
    if(depth_==depth){
        return;
    }
    depth_=depth;
    Sort();
}

void Camera::Sort() {
    std::sort(all_camera_.begin(),all_camera_.end(),[](Camera* a, Camera* b){
        return a->depth() < b->depth();
    });
}

void Camera::Foreach(std::function<void()> func) {
    for (auto iter=all_camera_.begin();iter!=all_camera_.end();iter++){
        current_camera_=*iter;
        current_camera_->CheckRenderToTexture();
        current_camera_->Clear();
        func();
        current_camera_->CheckCancelRenderToTexture();
    }
}











