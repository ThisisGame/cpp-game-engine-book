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

RigidBody::RigidBody(): px_rigid_actor_(nullptr), is_static_(false) {

}

RigidBody::~RigidBody(){

}

void RigidBody::Awake() {
    Transform* transform=dynamic_cast<Transform*>(game_object()->GetComponent("Transform"));
    if(is_static_){
        PxRigidStatic* px_rigid_static=Physics::CreateRigidStatic(transform->position(),game_object()->name());
        px_rigid_actor_=dynamic_cast<PxRigidActor*>(px_rigid_static);
    }else{
        PxRigidDynamic* px_rigid_dynamic=Physics::CreateRigidDynamic(transform->position(), game_object()->name());
        px_rigid_actor_=dynamic_cast<PxRigidActor*>(px_rigid_dynamic);
    }
}

void RigidBody::BindCollider(Collider *collider) {
    if (collider == nullptr){
        return;
    }
    PxShape* px_shape=collider->px_shape();
    px_rigid_actor_->attachShape(*px_shape);
    px_shape->release();
}

void RigidBody::Update() {

}

void RigidBody::FixedUpdate() {
    if(is_static_){
        return;
    }
    if(px_rigid_actor_ == nullptr){
        DEBUG_LOG_ERROR("px_rigid_body_== nullptr");
        return;
    }
    //PxRigidBody受Physx物理模拟驱动，位置被改变。获取最新的位置，去更新Transform。
    PxTransform px_transform=px_rigid_actor_->getGlobalPose();
    Transform* transform=dynamic_cast<Transform*>(game_object()->GetComponent("Transform"));
    transform->set_position(glm::vec3(px_transform.p.x,px_transform.p.y,px_transform.p.z));
}