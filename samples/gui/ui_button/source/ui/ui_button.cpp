//
// Created by captain on 2021/10/19.
//

#include "ui_button.h"
#include <rttr/registration>
#include "component/game_object.h"
#include "component/transform.h"
#include "renderer/texture2d.h"
#include "utils/debug.h"
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

    Transform* transform=dynamic_cast<Transform*>(game_object()->GetComponent("Transform"));

    if(Input::GetMouseButtonDown(MOUSE_BUTTON_LEFT)){
        auto transform_position=transform->position();
        auto mouse_position=Input::mousePosition();
        float image_normal_width=image_normal_->texture2D()->width();
        float image_normal_height=image_normal_->texture2D()->height();
        if((transform_position.x<mouse_position.x_ && mouse_position.x_<transform_position.x+image_normal_width)&&
            (transform_position.y<mouse_position.y_ && mouse_position.y_<transform_position.y+image_normal_height)){
            //隐藏
            DEBUG_LOG_INFO("click button");
        }
    }
}

void UIButton::OnPreRender() {
    Component::OnPreRender();
}

void UIButton::OnPostRender() {
    Component::OnPostRender();
}
