//
// Created by captainchen on 2022/6/6.
//

#include "light.h"
#include <rttr/registration>

using namespace rttr;
RTTR_REGISTRATION//注册反射
{
    registration::class_<Light>("Light")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

Light::Light():Component() {}

Light::~Light() {}