//
// Created by captain on 4/28/2022.
//

#ifndef INTEGRATE_PHYSX_SPHERE_COLLIDER_H
#define INTEGRATE_PHYSX_SPHERE_COLLIDER_H


#include "collider.h"

class SphereCollider : public Collider {
public:
    SphereCollider();
    ~SphereCollider();

protected:
    void CreateShape() override;

private:
    //~zh 球体碰撞器半径
    float radius_;
};


#endif //INTEGRATE_PHYSX_SPHERE_COLLIDER_H
