//
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

    Texture2D* vertex_position_texture_2d(){
        return vertex_position_texture_2d_;
    }
    Texture2D* vertex_normal_texture_2d(){
        return vertex_normal_texture_2d_;
    }
    Texture2D* vertex_color_texture_2d(){
        return vertex_color_texture_2d_;
    }
private:
    Texture2D* vertex_position_texture_2d_;//将FBO颜色附着点0关联的颜色纹理,存储着顶点坐标数据,绑定到FBO颜色附着点0
    Texture2D* vertex_normal_texture_2d_;//将FBO颜色附着点1关联的颜色纹理，存储着顶点法线数据,绑定到FBO颜色附着点1
    Texture2D* vertex_color_texture_2d_;//将FBO颜色附着点2关联的颜色纹理，存储着顶点颜色数据，绑定到FBO颜色附着点2
};


#endif //RENDER_TEXTURE_G_BUFFER_H
