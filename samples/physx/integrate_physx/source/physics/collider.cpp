//
// Created by captain on 4/28/2022.
//

#include "collider.h"

#include <rttr/registration>
#include "physics.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "utils/debug.h"
#include "physic_material.h"
#include "rigid_dynamic.h"

using namespace rttr;
RTTR_REGISTRATION{
    registration::class_<Collider>("Collider")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

Collider::Collider(): px_shape_(nullptr), px_material_(nullptr), physic_material_(nullptr), rigid_actor_(nullptr) {

}

Collider::~Collider(){

}

void Collider::Awake() {
    CreatePhysicMaterial();
    CreateShape();
    RegisterToRigidActor();
}

void Collider::CreatePhysicMaterial() {
    if(physic_material_== nullptr){
        physic_material_=new PhysicMaterial(0.5f,0.5f,0.5f);
    }
    if(px_material_== nullptr){
        px_material_=Physics::CreateMaterial(physic_material_->static_friction(),physic_material_->dynamic_friction(),physic_material_->restitution());
        px_material_->setRestitutionCombineMode(PxCombineMode::eAVERAGE);
    }
}

void Collider::CreateShape() {

}

void Collider::RegisterToRigidActor() {
    if(GetRigidActor() == nullptr){
        DEBUG_LOG_ERROR("rigid_actor_ is nullptr,Collider need to be attached to a rigid_actor");
        return;
    }
    rigid_actor_->BindCollider(this);
}

RigidActor * Collider::GetRigidActor() {
    if (rigid_actor_ != nullptr){
        return rigid_actor_;
    }
    auto component=game_object()->GetComponent("RigidDynamic");
    if(component != nullptr){
        rigid_actor_=dynamic_cast<RigidActor*>(component);
        return rigid_actor_;
    }
    component=game_object()->GetComponent("RigidStatic");
    if(component != nullptr){
        rigid_actor_=dynamic_cast<RigidActor*>(component);
        return rigid_actor_;
    }
    return nullptr;
}

void Collider::Update() {

}

void Collider::FixedUpdate() {

}