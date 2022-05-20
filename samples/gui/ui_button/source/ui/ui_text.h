//
// Created by captainchen on 2021/10/18.
//

#ifndef UNTITLED_UI_TEXT_H
#define UNTITLED_UI_TEXT_H

#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include "component/component.h"

class Font;
class MeshRenderer;
class UIText : public Component {
public:
    UIText();
    ~UIText();

    void set_font(Font* font){font_=font;}
    Font* font(){return font_;}

    void set_text(std::string text);
    std::string text(){return text_;}

    void set_color(glm::vec4 color){color_=color;}
    glm::vec4 color(){return color_;}
public:
    void Update() override;
    /// 渲染之前
    void OnPreRender() override;

    void OnPostRender() override;

private:
    Font* font_;
    std::string text_;
    bool dirty_;//是否变化需要重新生成Mesh
    glm::vec4 color_;//字体颜色
};


#endif //UNTITLED_UI_TEXT_H
