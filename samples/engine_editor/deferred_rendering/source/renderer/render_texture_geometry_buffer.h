﻿//
// Created by captainchen on 2022/8/19.
//

#ifndef RENDER_TEXTURE_G_BUFFER_H
#define RENDER_TEXTURE_G_BUFFER_H

#include "render_texture.h"

class Texture2D;
class RenderTextureGeometryBuffer: public RenderTexture{
public:
    RenderTextureGeometryBuffer();
    virtual ~RenderTextureGeometryBuffer();

    /// 初始化RenderTexture，在GPU生成帧缓冲区对象(FrameBufferObject)
    /// \param width
    /// \param height
    virtual void Init(unsigned short width,unsigned short height) override;

    Texture2D* frag_position_texture_2d(){
        return frag_position_texture_2d_;
    }
    Texture2D* frag_normal_texture_2d(){
        return frag_normal_texture_2d_;
    }
    Texture2D* frag_vertex_color_texture_2d(){
        return frag_vertex_color_texture_2d_;
    }
    Texture2D* frag_diffuse_color_texture_2d(){
        return frag_diffuse_color_texture_2d_;
    }
    Texture2D* frag_specular_intensity_texture_2d(){
        return frag_specular_intensity_texture_2d_;
    }
    Texture2D* frag_specular_highlight_shininess_texture_2d(){
        return frag_specular_highlight_shininess_texture_2d_;
    }
private:
    Texture2D* frag_position_texture_2d_;//将FBO颜色附着点0关联的颜色纹理,存储顶点片段坐标数据,绑定到FBO颜色附着点0
    Texture2D* frag_normal_texture_2d_;//将FBO颜色附着点1关联的颜色纹理，存储顶点片段法线数据,绑定到FBO颜色附着点1
    Texture2D* frag_vertex_color_texture_2d_;//将FBO颜色附着点2关联的颜色纹理，存储顶点片段颜色数据，绑定到FBO颜色附着点2
    Texture2D* frag_diffuse_color_texture_2d_;//将FBO颜色附着点3关联的颜色纹理，存储顶点片段Diffuse颜色数据，绑定到FBO颜色附着点3
    Texture2D* frag_specular_intensity_texture_2d_;//将FBO颜色附着点4关联的颜色纹理，存储顶点片段高光强度数据，绑定到FBO颜色附着点4
    Texture2D* frag_specular_highlight_shininess_texture_2d_;//将FBO颜色附着点5关联的颜色纹理，存储顶点片段反光度数据，绑定到FBO颜色附着点5
};


#endif //RENDER_TEXTURE_G_BUFFER_H
