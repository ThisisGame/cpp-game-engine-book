//
// Created by captain on 2021/6/9.
//

#include "component.h"

//注册反射
RTTR_REGISTRATION
{
    registration::class_<Component>("Component")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

Component::Component() {

}

Component::~Component() {

}

void Component::OnEnable() {

}

void Component::Awake() {

}

void Component::Update() {

}

void Component::OnPreRender() {

}

void Component::OnPostRender() {

}

void Component::OnDisable() {

}
