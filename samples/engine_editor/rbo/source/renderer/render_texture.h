//
// Created by captainchen on 2022/8/19.
//

#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H
#include "render_target.h"

class Texture2D;
class RenderTexture:public RenderTarget {
public:
    RenderTexture();
    ~RenderTexture();

    virtual void Init(unsigned short width,unsigned short height) override;

    Texture2D* color_texture_2d(){
        return color_texture_2d_;
    }
    Texture2D* depth_texture_2d(){
        return depth_texture_2d_;
    }
private:
    Texture2D* color_texture_2d_;//FBO颜色附着点关联的颜色纹理
    Texture2D* depth_texture_2d_;//FBO深度附着点关联的深度纹理
};


#endif //RENDER_TEXTURE_H
