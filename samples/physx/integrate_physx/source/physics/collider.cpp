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
#include "rigid_body.h"

using namespace rttr;
RTTR_REGISTRATION{
    registration::class_<Collider>("Collider")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

Collider::Collider():px_shape_(nullptr),px_material_(nullptr),physic_material_(nullptr),rigid_body_(nullptr) {

}

Collider::~Collider(){

}

void Collider::Awake() {
    GetOrAddRigidBody();
    CreatePhysicMaterial();
    CreateShape();
    RegisterToRigidBody();
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

void Collider::RegisterToRigidBody() {
    if(rigid_body_== nullptr){
        DEBUG_LOG_ERROR("rigid_body_ is nullptr");
        return;
    }
    rigid_body_->BindCollider(this);
}

void Collider::GetOrAddRigidBody() {
    if(rigid_body_== nullptr){
        auto component=game_object()->GetComponent("RigidBody");
        if(component== nullptr){
            DEBUG_LOG_INFO("RigidBody not found,add it to game object");
            component = game_object()->AddComponent("RigidBody");
        }
        if(component!= nullptr){
            rigid_body_=dynamic_cast<RigidBody*>(component);
        }
        if(rigid_body_== nullptr){
            DEBUG_LOG_ERROR("RigidBody not found");
        }
    }
}

void Collider::Update() {

}

void Collider::FixedUpdate() {

}