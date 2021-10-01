//
// Created by captain on 2021/4/5.
// 图片加载与解析
//

#ifndef UNTITLED_TEXTURE2D_H
#define UNTITLED_TEXTURE2D_H

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <glad/gl.h>

class Texture2D
{
private:
    Texture2D();

public:
    ~Texture2D();

    /// 加载一个图片文件
    /// \param image_file_path
    /// \return
    static Texture2D* LoadFromFile(std::string image_file_path);

    /// 创建Texture(不压缩)
    /// \param width
    /// \param height
    /// \param internal_format 在显卡中储存的格式
    /// \param data
    /// \return
    static Texture2D* Create(unsigned short width,unsigned short height,unsigned int internal_format,unsigned int data_type,unsigned char* data);

public:
    //cpt文件头
    struct CptFileHead
    {
        char type_[3];
        int mipmap_level_;
        int width_;
        int height_;
        int gl_texture_format_;
        int compress_size_;
    };

    int mipmap_level(){return mipmap_level_;}
    int width(){return width_;}
    int height(){return height_;}
    GLenum gl_texture_format(){return gl_texture_format_;}
    GLuint gl_texture_id(){return gl_texture_id_;}

private:
    int mipmap_level_;
    int width_;
    int height_;
    GLenum gl_texture_format_;
    GLuint gl_texture_id_;//纹理ID
};

#endif //UNTITLED_TEXTURE2D_H
