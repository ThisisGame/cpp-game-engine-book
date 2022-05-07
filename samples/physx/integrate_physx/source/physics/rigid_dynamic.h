//
// Created by cp on 2022/4/23.
//

#ifndef UNTITLED_RIGID_DYNAMIC_H
#define UNTITLED_RIGID_DYNAMIC_H

#include "rigid_actor.h"

class Collider;
class RigidDynamic : public RigidActor{
public:
    RigidDynamic();
    ~RigidDynamic();

    void BindCollider(Collider *collider) override;
public:
    /// Awake里反序列化给成员变量赋值。
    void Awake() override;

    void Update() override;

    void FixedUpdate() override;
};


#endif //UNTITLED_RIGID_DYNAMIC_H
