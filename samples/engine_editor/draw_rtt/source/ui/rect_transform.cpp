//
// Created by captain on 8/24/2022.
//

#include "rect_transform.h"
#include <rttr/registration>

using namespace rttr;
RTTR_REGISTRATION//注册反射
{
    registration::class_<RectTransform>("RectTransform")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

RectTransform::RectTransform():Transform(){

}

RectTransform::~RectTransform(){

}

