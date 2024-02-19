//
// Created by captainchen on 2022/8/19.
//

#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

class Texture2D;
class RenderTexture {
public:
    RenderTexture();
    ~RenderTexture();

    /// 初始化RenderTexture，在GPU生成帧缓冲区对象(FrameBufferObject)
    /// \param width
    /// \param height
    void Init(unsigned short width,unsigned short height);

    unsigned short width(){
        return width_;
    }
    void set_width(unsigned short width){
        width_=width;
    }

    unsigned short height(){
        return height_;
    }
    void set_height(unsigned short height){
        height_=height;
    }

    unsigned int frame_buffer_object_handle(){
        return frame_buffer_object_handle_;
    }
    /// 是否正在被使用
    bool in_use(){
        return in_use_;
    }
    void set_in_use(bool in_use){
        in_use_=in_use;
    }
    Texture2D* color_texture_2d(){
        return color_texture_2d_;
    }
    Texture2D* depth_texture_2d(){
        return depth_texture_2d_;
    }

protected:
    unsigned short width_;
    unsigned short height_;
    unsigned int frame_buffer_object_handle_;//关联的FBO Handle
    Texture2D* color_texture_2d_;//FBO颜色附着点关联的颜色纹理
    Texture2D* depth_texture_2d_;//FBO深度附着点关联的深度纹理
    bool in_use_;//正在被使用
};


#endif //RENDER_TEXTURE_H
