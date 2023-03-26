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

    void UpdateCCDState();

    bool enable_ccd(){return enable_ccd_;}
    void set_enable_ccd(bool enable_ccd){
        enable_ccd_=enable_ccd;
        UpdateCCDState();
    }

public:
    /// Awake里反序列化给成员变量赋值。
    void Awake() override;

    void Update() override;

    void FixedUpdate() override;

private:
    bool enable_ccd_;

RTTR_ENABLE(RigidActor)
};


#endif //UNTITLED_RIGID_DYNAMIC_H
