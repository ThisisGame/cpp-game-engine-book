//
// Created by captain on 2021/10/19.
//

#ifndef UNTITLED_UI_BUTTON_H
#define UNTITLED_UI_BUTTON_H

#include <iostream>
#include <string>
#include <functional>
#include "component/component.h"

class UIImage;
class UIButton : public Component {
public:
    UIButton();
    ~UIButton();

    void set_image_normal(UIImage* image){image_normal_=image;}
    void set_image_press(UIImage* image){image_press_=image;}
    /// 设置按钮点击回调
    /// \param callback
    void set_click_callback(std::function<void()> click_callback){click_callback_=click_callback;}
public:
    void Update() override;
private:
    UIImage* image_normal_;//普通状态显示图片
    UIImage* image_press_;//按压状态显示图片
    bool pressed_;//当前按钮状态是否按下
    std::function<void()> click_callback_;//按钮点击回调
};


#endif //UNTITLED_UI_BUTTON_H
