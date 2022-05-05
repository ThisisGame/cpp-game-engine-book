//
// Created by cp on 2022/4/23.
//

#ifndef UNTITLED_RIGID_BODY_H
#define UNTITLED_RIGID_BODY_H

#include <PxPhysicsAPI.h>
#include "component/component.h"

using namespace physx;

class Collider;
//~zh 刚体组件，对Physx PxRigidBody进行封装。Collider组件依赖于刚体组件，所以当有Collider组件时，刚体组件也必须存在。
class RigidBody : public Component{
public:
    RigidBody();
    ~RigidBody();

    void Create();

    void BindCollider(Collider *collider);

    bool is_static(){return is_static_;}
    void set_is_static(bool is_static);
public:
    /// Awake里反序列化给成员变量赋值。
    void Awake() override;

    void Update() override;

    void FixedUpdate() override;

private:
    PxRigidActor*    px_rigid_actor_;

    bool            is_static_;//是否是静态对象。
};


#endif //UNTITLED_RIGID_BODY_H
