//
// Created by captain on 2021/9/23.
//

#include "animator.h"

//注册反射
RTTR_REGISTRATION
{
    registration::class_<Animator>("Animator")
        .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}