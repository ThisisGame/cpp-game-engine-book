//
// Created by captain on 2021/6/9.
//

#include "transform.h"
#include "rttr/registration"

using namespace rttr;
RTTR_REGISTRATION//注册反射
{
    registration::class_<Transform>("Transform")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr)
            .property("position", &Transform::position, &Transform::set_position)
            .property("rotation", &Transform::rotation, &Transform::set_rotation)
            .property("scale", &Transform::scale, &Transform::set_scale);
}

Transform::Transform():Component(),position_(0.f),rotation_(0.f),scale_(1.f) {
}

Transform::~Transform() {
}
