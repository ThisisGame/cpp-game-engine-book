//
// Created by captain on 2021/9/23.
//

#include "camera.h"

//注册反射
RTTR_REGISTRATION
{
    registration::class_<Camera>("Camera")
        .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}
