//
// Created by cp on 2022/4/23.
//

#include "rigid_body.h"
#include <rttr/registration>
#include "physics.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "utils/debug.h"
#include "collider.h"

using namespace rttr;
RTTR_REGISTRATION{
    registration::class_<RigidBody>("RigidBody")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

RigidBody::RigidBody():px_rigid_body_(nullptr),is_static_(false) {

}

RigidBody::~RigidBody(){

}

void RigidBody::Awake() {

}

void RigidBody::BindCollider(Collider *collider) {
    if (collider == nullptr){
        return;
    }
    PxShape* px_shape=collider->px_shape();
    px_rigid_body_->attachShape(*px_shape);
    px_shape->release();
}

void RigidBody::Update() {
    if(px_rigid_body_== nullptr){
        Transform* transform=dynamic_cast<Transform*>(game_object()->GetComponent("Transform"));
        if(is_static_){
            PxRigidStatic* px_rigid_static=Physics::CreateRigidStatic(transform->position(),game_object()->name());
            px_rigid_body_=dynamic_cast<PxRigidBody*>(px_rigid_static);
        }else{
            PxRigidDynamic* px_rigid_dynamic=Physics::CreateRigidDynamic(transform->position(), game_object()->name());
            px_rigid_body_=dynamic_cast<PxRigidBody*>(px_rigid_dynamic);
        }
    }
}

void RigidBody::FixedUpdate() {
    if(is_static_){
        return;
    }
    if(px_rigid_body_== nullptr){
        DEBUG_LOG_ERROR("px_rigid_body_== nullptr");
        return;
    }
    //PxRigidBody受Physx物理模拟驱动，位置被改变。获取最新的位置，去更新Transform。
    PxTransform px_transform=px_rigid_body_->getGlobalPose();
    Transform* transform=dynamic_cast<Transform*>(game_object()->GetComponent("Transform"));
    transform->set_position(glm::vec3(px_transform.p.x,px_transform.p.y,px_transform.p.z));
}