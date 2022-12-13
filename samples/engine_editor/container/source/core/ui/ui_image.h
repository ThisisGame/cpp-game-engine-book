//
// Created by captain on 2021/10/14.
//

#ifndef UNTITLED_UI_IMAGE_H
#define UNTITLED_UI_IMAGE_H

#include "core/component/component.h"
#include "core/renderer/mesh_filter.h"

class Texture2D;
class UIImage : public Component {
public:
    UIImage();
    ~UIImage() override;

    Texture2D* texture2D(){return texture2D_;}
    void set_texture(Texture2D* texture2D){
        texture2D_=texture2D;
    }

    /// 指定图片路径加载并设置
    /// \param texture_file_path
    void LoadTexture2D(const char* texture_file_path);
public:
    void Update() override;
    /// 渲染之前
    void OnPreRender() override;

    void OnPostRender() override;
private:
    Texture2D* texture2D_= nullptr;//Texture

RTTR_ENABLE();
};


#endif //UNTITLED_UI_IMAGE_H
