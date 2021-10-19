//
// Created by captain on 2021/10/19.
//

#include "ui_button.h"
#include <rttr/registration>
#include <glm/ext.hpp>
#include "component/game_object.h"
#include "component/transform.h"
#include "renderer/texture2d.h"
#include "utils/debug.h"
#include "utils/screen.h"
#include "control/input.h"
#include "control/key_code.h"
#include "ui_image.h"

using namespace rttr;
RTTR_REGISTRATION{
    registration::class_<UIButton>("UIButton")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

UIButton::UIButton() {

}

UIButton::~UIButton() {

}

void UIButton::Update() {
    Component::Update();

    if(image_normal_== nullptr){
        return;
    }

    Transform* transform_image_normal=dynamic_cast<Transform*>(image_normal_->game_object()->GetComponent("Transform"));
    Transform* transform=dynamic_cast<Transform*>(game_object()->GetComponent("Transform"));
    transform_image_normal->set_position(transform->position());

    if(Input::GetMouseButtonDown(MOUSE_BUTTON_LEFT)){
        auto transform_position=transform->position();
        auto mouse_position=Input::mousePosition();
        mouse_position.x=mouse_position.x-Screen::width()/2;
        mouse_position.y=Screen::height()/2-mouse_position.y;
        float image_normal_width=image_normal_->texture2D()->width();
        float image_normal_height=image_normal_->texture2D()->height();
        if((mouse_position.x>transform_position.x && mouse_position.x<transform_position.x+image_normal_width)&&
            (mouse_position.y>transform_position.y && mouse_position.y<transform_position.y+image_normal_height)){
            //隐藏
            DEBUG_LOG_INFO("click button:{}",glm::to_string(mouse_position));
        }
    }
}

void UIButton::OnPreRender() {
    Component::OnPreRender();
}

void UIButton::OnPostRender() {
    Component::OnPostRender();
}
