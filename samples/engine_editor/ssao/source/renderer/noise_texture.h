//
// Created by captainchen on 2022/8/19.
//

#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

class Texture2D;
class NoiseTexture {
public:
    NoiseTexture();
    virtual ~NoiseTexture();

    /// 初始化NoiseTexture，在GPU生成帧缓冲区对象(FrameBufferObject)
    /// \param width
    /// \param height
    virtual void Init(unsigned short width,unsigned short height);

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

    /// 是否正在被使用
    bool in_use(){
        return in_use_;
    }
    void set_in_use(bool in_use){
        in_use_=in_use;
    }
    Texture2D* noise_texture_2d(){
        return noise_texture_2d_;
    }

protected:
    unsigned short width_;
    unsigned short height_;
    Texture2D* noise_texture_2d_;
    bool in_use_;//正在被使用
};


#endif //RENDER_TEXTURE_H
