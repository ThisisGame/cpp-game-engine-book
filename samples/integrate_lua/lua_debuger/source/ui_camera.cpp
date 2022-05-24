//
// Created by captainchen on 2021/12/6.
//

#include "ui_camera.h"

//注册反射
RTTR_REGISTRATION
{
    registration::class_<UICamera>("UICamera")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

UICamera::UICamera(): Camera() {

}

UICamera::~UICamera() {

}