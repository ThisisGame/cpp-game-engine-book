//
// Created by captain on 4/28/2022.
//

#ifndef INTEGRATE_PHYSX_BOX_COLLIDER_H
#define INTEGRATE_PHYSX_BOX_COLLIDER_H


#include "collider.h"
#include <glm/glm.hpp>

class BoxCollider : public Collider {
public:
    BoxCollider();
    ~BoxCollider();

protected:
    void CreateShape() override;

private:
    //~zh 碰撞器尺寸
    glm::vec3 size_;
};


#endif //INTEGRATE_PHYSX_BOX_COLLIDER_H
