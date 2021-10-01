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

    /// 部分更新Texture
    /// \param x
    /// \param y
    /// \param width
    /// \param height
    /// \param client_format
    /// \param data_type
    /// \param data
    void UpdateSubImage(int x,int y,int width,int height,unsigned int client_format,unsigned int data_type,unsigned char* data);

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

public:
    /// 加载一个图片文件
    /// \param image_file_path
    /// \return
    static Texture2D* LoadFromFile(std::string image_file_path);

    /// 创建Texture(不压缩)
    /// \param width
    /// \param height
    /// \param server_format 在显存中储存的格式
    /// \param client_format 在内存中储存的格式
    /// \param data_type 上传的数据类型
    /// \param data 上传的图像数据，函数里不会删除这份数据，需要自己管理。
    /// \return
    static Texture2D* Create(unsigned short width,unsigned short height,unsigned int server_format,unsigned int client_format,unsigned int data_type,unsigned char* data);
};

#endif //UNTITLED_TEXTURE2D_H
