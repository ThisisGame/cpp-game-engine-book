//
// Created by captain on 2021/4/6.
// 图片加载与解析
//
#include "texture2d.h"

#include "stb/stb_image.h"



Texture2D* Texture2D::LoadFromFile(std::string& image_file_path)
{
    Texture2D* texture2d=new Texture2D();

    stbi_set_flip_vertically_on_load(true);//翻转图片，解析出来的图片数据从左下角开始，这是因为OpenGL的纹理坐标起始点为左下角。
    int channels_in_file;//通道数
    unsigned char* data = stbi_load(image_file_path.c_str(), &(texture2d->width_), &(texture2d->height_), &channels_in_file, 0);
    if (data!= nullptr)
    {
        //根据颜色通道数，判断颜色格式。
        switch (channels_in_file) {
            case 1:
            {
                texture2d->gl_texture_format_ = GL_ALPHA;
                break;
            }
            case 3:
            {
                texture2d->gl_texture_format_ = GL_RGB;
                break;
            }
            case 4:
            {
                texture2d->gl_texture_format_ = GL_RGBA;
                break;
            }
        }
    }

    //释放图片文件内存
    stbi_image_free(data);

    return texture2d;
}
