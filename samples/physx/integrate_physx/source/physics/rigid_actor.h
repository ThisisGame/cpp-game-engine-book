//
// Created by captainchen on 2022/5/7.
//

#ifndef INTEGRATE_PHYSX_RIGID_ACTOR_H
#define INTEGRATE_PHYSX_RIGID_ACTOR_H

#include <PxPhysicsAPI.h>
#include "component/component.h"

using namespace physx;

class Collider;
class RigidActor : public Component{
public:
    RigidActor();
    ~RigidActor();

    virtual void BindCollider(Collider *collider);

public:
    void Awake() override;

protected:
    PxRigidActor* px_rigid_actor_;
};


#endif //INTEGRATE_PHYSX_RIGID_ACTOR_H
