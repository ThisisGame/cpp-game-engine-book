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

UIButton::UIButton():pressed_(false),click_callback_(nullptr) {

}

UIButton::~UIButton() {

}

void UIButton::Update() {
    Component::Update();

    if(image_normal_== nullptr || image_press_== nullptr){
        return;
    }

    Transform* transform=game_object()->GetComponent<Transform>();
    //按钮的图片 与 按钮坐标同步
    Transform* transform_image_normal=image_normal_->game_object()->GetComponent<Transform>();
    Transform* transform_image_press=image_press_->game_object()->GetComponent<Transform>();
    transform_image_normal->set_position(transform->position());
    transform_image_press->set_position(transform->position());

    if(Input::GetMouseButtonDown(MOUSE_BUTTON_LEFT)){
        auto transform_position=transform->position();
        //获取鼠标坐标，并从左上角零点，转换为屏幕中心零点。
        auto mouse_position=Input::mousePosition();
        mouse_position.x=mouse_position.x-Screen::width()/2;
        mouse_position.y=Screen::height()/2-mouse_position.y;
        //获取按钮图片宽高
        float image_normal_width=image_normal_->texture2D()->width();
        float image_normal_height=image_normal_->texture2D()->height();
        //计算鼠标点击是否在按钮图片范围内
        if((mouse_position.x>transform_position.x && mouse_position.x<transform_position.x+image_normal_width)&&
            (mouse_position.y>transform_position.y && mouse_position.y<transform_position.y+image_normal_height)){
            if(pressed_){
                return;
            }
            pressed_=true;
            //触发按钮点击回调
            if(click_callback_){
                click_callback_();
            }
        }else{
            pressed_=false;
        }
    }else{
        pressed_=false;
    }
    //当press为true 关闭普通状态图片，激活按下状态图片.否则反转。
    transform_image_normal->game_object()->set_active(!pressed_);
    transform_image_press->game_object()->set_active(pressed_);
}
