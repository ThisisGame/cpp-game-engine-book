//
// Created by captain on 4/28/2022.
//

#include "box_collider.h"
#include "rttr/registration"
#include "physics.h"

using namespace rttr;
RTTR_REGISTRATION//注册反射
{
    registration::class_<BoxCollider>("BoxCollider")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

BoxCollider::BoxCollider():Collider(),size_(1,1,1)
{

}

BoxCollider::~BoxCollider() {

}

void BoxCollider::CreateShape() {
    if(px_shape_== nullptr){
        px_shape_=Physics::CreateBoxShape(size_,px_material_);
    }
}