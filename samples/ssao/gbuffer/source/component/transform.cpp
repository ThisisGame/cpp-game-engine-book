//
// Created by captain on 2021/6/9.
//

#include "transform.h"
#include "game_object.h"
#include <rttr/registration>


using namespace rttr;
RTTR_REGISTRATION//注册反射
{
    registration::class_<Transform>("Transform")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr)
            .property("local_position", &Transform::local_position, &Transform::set_local_position)
            .property("local_rotation", &Transform::local_rotation, &Transform::set_local_rotation)
            .property("local_scale", &Transform::local_scale, &Transform::set_local_scale);
}

Transform::Transform(): Component(), local_position_(0.f), local_rotation_(0.f), local_scale_(1.f) {
}

Transform::~Transform() {
}

glm::vec3 Transform::position() {
    glm::vec3 position=local_position();

    // 递归获取父节点的位置
    GameObject* parent_game_object= dynamic_cast<GameObject *>(game_object()->parent());
    while(parent_game_object!= nullptr){
        Transform* parent_transform=parent_game_object->GetComponent<Transform>();
        if(parent_transform!= nullptr){
            position+=parent_transform->local_position();
        }
        parent_game_object=dynamic_cast<GameObject *>(parent_game_object->parent());
    }
    return position;
}

glm::vec3 Transform::rotation() {
    glm::vec3 rotation=local_rotation();

    // 递归获取父节点的旋转
    GameObject* parent_game_object= dynamic_cast<GameObject *>(game_object()->parent());
    while(parent_game_object!= nullptr){
        Transform* parent_transform=parent_game_object->GetComponent<Transform>();
        if(parent_transform!= nullptr){
            rotation+=parent_transform->local_rotation();
        }
        parent_game_object=dynamic_cast<GameObject *>(parent_game_object->parent());
    }
    return rotation;
}

glm::vec3 Transform::scale() {
    glm::vec3 scale=local_scale();

    // 递归获取父节点的缩放
    GameObject* parent_game_object= dynamic_cast<GameObject *>(game_object()->parent());
    while(parent_game_object!= nullptr){
        Transform* parent_transform=parent_game_object->GetComponent<Transform>();
        if(parent_transform!= nullptr){
            scale=parent_transform->local_scale() * scale;
        }
        parent_game_object=dynamic_cast<GameObject *>(parent_game_object->parent());
    }
    return scale;
}
