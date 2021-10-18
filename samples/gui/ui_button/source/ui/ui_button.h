//
// Created by captain on 2021/10/19.
//

#ifndef UNTITLED_UI_BUTTON_H
#define UNTITLED_UI_BUTTON_H

#include <iostream>
#include <string>
#include "component/component.h"

class UIImage;
class UIButton : public Component {
public:
    UIButton();
    ~UIButton();

    void set_image_normal(UIImage* image){image_normal_=image;}
    void set_image_press(UIImage* image){image_press_=image;}
public:
    void Update() override;
    /// 渲染之前
    void OnPreRender() override;

    void OnPostRender() override;

private:
    UIImage* image_normal_;//普通状态显示图片
    UIImage* image_press_;//按压状态显示图片
};


#endif //UNTITLED_UI_BUTTON_H
