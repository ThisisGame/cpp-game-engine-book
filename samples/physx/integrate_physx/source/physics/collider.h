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

public:
    /// Awake里反序列化给成员变量赋值。
    void Awake() override;

    void Update() override;

    void FixedUpdate() override;

public:
    virtual void CreatePhysicMaterial();
    virtual void CreateShape();
    virtual void RegisterToRigidActor();

private:
    RigidActor * GetRigidActor();

protected:
    PxShape* px_shape_;
    PxMaterial* px_material_;
private:
    PhysicMaterial* physic_material_;
    RigidActor* rigid_actor_;

RTTR_ENABLE();
};


#endif //INTEGRATE_PHYSX_COLLIDER_H
