//
// Created by captain on 4/28/2022.
//

#ifndef INTEGRATE_PHYSX_COLLIDER_H
#define INTEGRATE_PHYSX_COLLIDER_H

#include <PxPhysicsAPI.h>
#include "component/component.h"

using namespace physx;

class RigidBody;
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

protected:
    virtual void CreatePhysicMaterial();
    virtual void CreateShape();
    virtual void RegisterToRigidBody();

private:
    void GetOrAddRigidBody();

protected:
    PxShape* px_shape_;
    PxMaterial* px_material_;
private:
    PhysicMaterial* physic_material_;
    RigidBody* rigid_body_;
};


#endif //INTEGRATE_PHYSX_COLLIDER_H
