//
// Created by cp on 2022/4/23.
//

#ifndef UNTITLED_RIGID_BODY_H
#define UNTITLED_RIGID_BODY_H

#include <PxPhysicsAPI.h>
#include "component/component.h"

using namespace physx;

class Collider;
// 刚体
class RigidBody : public Component{
public:
    RigidBody();
    ~RigidBody();

    void BindCollider(Collider *collider);

public:
    /// Awake里反序列化给成员变量赋值。
    void Awake() override;

    void Update() override;

    void FixedUpdate() override;

private:
    PxRigidBody*    px_rigid_body_;

    bool            is_static_;//是否是静态对象。
};


#endif //UNTITLED_RIGID_BODY_H
