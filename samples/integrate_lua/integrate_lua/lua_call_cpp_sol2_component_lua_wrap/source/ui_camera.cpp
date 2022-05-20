//
// Created by captainchen on 2021/12/6.
//

#include "ui_camera.h"

#include <rttr/registration>
using namespace rttr;

//注册反射
RTTR_REGISTRATION
{
    registration::class_<UICamera>("UICamera")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

UICamera::UICamera(): Camera() {
    set_type_name("UICamera");
}

UICamera::~UICamera() {

}