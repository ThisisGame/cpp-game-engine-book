//
// Created by captain on 2021/10/15.
//

#ifndef UNTITLED_UI_MASK_H
#define UNTITLED_UI_MASK_H

#include "component/component.h"
#include "renderer/mesh_filter.h"

class Texture2D;
class UIMask : public Component {
public:
    UIMask();
    ~UIMask() override;

    Texture2D* texture2D(){return texture2D_;}
    void set_texture(Texture2D* texture2D){texture2D_=texture2D;}

public:
    void OnEnable() override;

    void Update() override;
    /// 渲染之前
    void OnPreRender() override;
    /// 渲染之后
    void OnPostRender() override;

    void OnDisable() override;
private:
    Texture2D* texture2D_= nullptr;//Texture

RTTR_ENABLE();
};


#endif //UNTITLED_UI_MASK_H
