//
// Created by captain on 4/28/2022.
//

#ifndef INTEGRATE_PHYSX_COLLIDER_H
#define INTEGRATE_PHYSX_COLLIDER_H

#include <rttr/registration>
#include <PxPhysicsAPI.h>
#include "component/component.h"

using namespace rttr;
using namespace physx;

class RigidActor;
class PhysicMaterial;
class Collider : public Component {
public:
    Collider();
    ~Collider();

    PxShape* px_shape(){ return px_shape_; }

    bool is_trigger(){return is_trigger_;}
    void set_is_trigger(bool is_trigger){
        if(is_trigger_==is_trigger){
            return;
        }
        is_trigger_=is_trigger;
        UnRegisterToRigidActor();
        CreateShape();
        UpdateTriggerState();
        RegisterToRigidActor();
    }

public:
    /// Awake里反序列化给成员变量赋值。
    void Awake() override;

    void Update() override;

    void FixedUpdate() override;

    virtual void CreatePhysicMaterial();
    virtual void CreateShape();
    /// 更新Shape 触发器 Filter
    virtual void UpdateTriggerState();
    virtual void RegisterToRigidActor();
    virtual void UnRegisterToRigidActor();

private:
    RigidActor * GetRigidActor();

protected:
    PxShape* px_shape_;
    PxMaterial* px_material_;
    bool is_trigger_;//是触发器，触发器只检测碰撞，而不进行物理模拟。

private:
    PhysicMaterial* physic_material_;
    RigidActor* rigid_actor_;

RTTR_ENABLE();
};


#endif //INTEGRATE_PHYSX_COLLIDER_H
