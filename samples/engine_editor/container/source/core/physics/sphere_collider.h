//
// Created by captain on 4/28/2022.
//

#ifndef INTEGRATE_PHYSX_SPHERE_COLLIDER_H
#define INTEGRATE_PHYSX_SPHERE_COLLIDER_H

#include "rttr/registration"
#include "collider.h"

using namespace rttr;

class SphereCollider : public Collider {
public:
    SphereCollider();
    ~SphereCollider();
protected:
    void CreateShape() override;

private:
    //~zh 球体碰撞器半径
    float radius_;

RTTR_ENABLE(Collider);
};


#endif //INTEGRATE_PHYSX_SPHERE_COLLIDER_H
