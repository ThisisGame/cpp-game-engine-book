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
    /// 从ttf字体创建贴图
    /// \param ttf_file_path
    /// \param word
    /// \return
    static Texture2D* CreateFromTrueTypeFont(std::string ttf_file_path, const char* word);
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
