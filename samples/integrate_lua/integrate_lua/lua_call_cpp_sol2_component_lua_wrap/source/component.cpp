//
// Created by captainchen on 2021/9/16.
//

#include "component.h"

//注册反射
RTTR_REGISTRATION
{
    registration::class_<Component>("Component")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}