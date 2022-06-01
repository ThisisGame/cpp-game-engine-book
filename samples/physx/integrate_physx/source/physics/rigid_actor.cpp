//
// Created by cp on 2022/4/23.
//

#include "rigid_actor.h"
#include <rttr/registration>
#include "physics.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "utils/debug.h"
#include "collider.h"

using namespace rttr;
RTTR_REGISTRATION{
    registration::class_<RigidActor>("RigidActor")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

RigidActor::RigidActor(): px_rigid_actor_(nullptr){

}

RigidActor::~RigidActor(){

}

void RigidActor::Awake() {
    Component* component=game_object()->GetComponent<Collider>();
    if(component!= nullptr){
        Collider* collider=static_cast<Collider*>(component);
        BindCollider(collider);
    }
}

void RigidActor::BindCollider(Collider *collider) {
    if (collider == nullptr){
        return;
    }
    PxShape* px_shape=collider->px_shape();
    px_rigid_actor_->attachShape(*px_shape);
    px_shape->release();
}