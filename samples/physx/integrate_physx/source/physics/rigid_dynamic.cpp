//
// Created by cp on 2022/4/23.
//

#include "rigid_dynamic.h"
#include <rttr/registration>
#include "physics.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "utils/debug.h"
#include "collider.h"

using namespace rttr;
RTTR_REGISTRATION{
    registration::class_<RigidDynamic>("RigidDynamic")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

RigidDynamic::RigidDynamic(){

}

RigidDynamic::~RigidDynamic(){

}

void RigidDynamic::Awake() {
    Transform* transform=dynamic_cast<Transform*>(game_object()->GetComponent("Transform"));
    PxRigidDynamic* px_rigid_dynamic=Physics::CreateRigidDynamic(transform->position(), game_object()->name());
    px_rigid_actor_=dynamic_cast<PxRigidActor*>(px_rigid_dynamic);
}

void RigidDynamic::Update() {

}

void RigidDynamic::FixedUpdate() {
    if(px_rigid_actor_ == nullptr){
        DEBUG_LOG_ERROR("px_rigid_actor_== nullptr");
        return;
    }
    //PxRigidBody受Physx物理模拟驱动，位置被改变。获取最新的位置，去更新Transform。
    PxTransform px_transform=px_rigid_actor_->getGlobalPose();
    Transform* transform=dynamic_cast<Transform*>(game_object()->GetComponent("Transform"));
    transform->set_position(glm::vec3(px_transform.p.x,px_transform.p.y,px_transform.p.z));
}