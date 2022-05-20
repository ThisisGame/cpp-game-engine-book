//
// Created by captain on 2021/10/14.
//

#ifndef UNTITLED_UI_IMAGE_H
#define UNTITLED_UI_IMAGE_H

#include "component/component.h"
#include "renderer/mesh_filter.h"
#include "renderer/texture2d.h"


class UIImage : public Component {
public:
    UIImage();
    ~UIImage() override;

    Texture2D* texture2D(){return texture2D_;}
    void set_texture(Texture2D* texture2D){
        texture2D_=texture2D;
        width_=texture2D->width();
        height_=texture2D->height();
    }

    int width(){return width_;}
    int height(){return height_;}

public:
    void Update() override;
    /// 渲染之前
    void OnPreRender() override;

    void OnPostRender() override;
private:
    Texture2D* texture2D_;//Texture
    int width_;
    int height_;
};


#endif //UNTITLED_UI_IMAGE_H
