//
// Created by captain on 4/28/2022.
//

#ifndef INTEGRATE_PHYSX_BOX_COLLIDER_H
#define INTEGRATE_PHYSX_BOX_COLLIDER_H

#include <rttr/registration>
#include <glm/glm.hpp>
#include "collider.h"

using namespace rttr;

class BoxCollider : public Collider {
public:
    BoxCollider();
    ~BoxCollider();

protected:
    void CreateShape() override;

private:
    //~zh 碰撞器尺寸
    glm::vec3 size_;

RTTR_ENABLE(Collider);
};


#endif //INTEGRATE_PHYSX_BOX_COLLIDER_H
