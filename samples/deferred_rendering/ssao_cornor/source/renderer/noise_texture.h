//
// Created by captainchen on 2022/8/19.
//

#ifndef NOISE_TEXTURE_H
#define NOISE_TEXTURE_H

#include<vector>
#include <glm/glm.hpp>

class Texture2D;
class NoiseTexture {
public:
    NoiseTexture();
    virtual ~NoiseTexture();

    /// 初始化NoiseTexture，在GPU生成帧缓冲区对象(FrameBufferObject)
    /// \param width
    /// \param height
    /// \param noise 生成的噪声
    virtual void Init(unsigned short width,unsigned short height,std::vector<glm::vec3> noise);

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


#endif //NOISE_TEXTURE_H
