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
    Texture2D(){};
    ~Texture2D(){};

public:
    static Texture2D* LoadFromFile(std::string& image_file_path);//加载一个图片文件

public:
    int width_;
    int height_;

    GLenum gl_texture_format_;
    GLuint gl_texture_id_;//纹理ID
};

#endif //UNTITLED_TEXTURE2D_H
