//
// Created by captain on 2021/4/6.
//
#include "texture2d.h"
#include <fstream>
#include "stb/stb_image.h"
#include "timetool/stopwatch.h"

using timetool::StopWatch;
using std::ofstream;
using std::ios;

Texture2D* Texture2D::LoadFromFile(std::string& image_file_path)
{
    Texture2D* texture2d=new Texture2D();

    stbi_set_flip_vertically_on_load(true);//翻转图片，解析出来的图片数据从左下角开始，这是因为OpenGL的纹理坐标起始点为左下角。
    int channels_in_file;//通道数

    StopWatch stopwatch;
    stopwatch.start();
    unsigned char* data = stbi_load(image_file_path.c_str(), &(texture2d->width_), &(texture2d->height_), &channels_in_file, 0);
    stopwatch.stop();
    std::int64_t decompress_jpg_cost = stopwatch.milliseconds();

    int image_data_format=GL_RGB;
    if (data!= nullptr)
    {
        //根据颜色通道数，判断颜色格式。
        switch (channels_in_file) {
            case 1:
            {
                image_data_format = GL_ALPHA;
                break;
            }
            case 3:
            {
                image_data_format = GL_RGB;
                texture2d->gl_texture_format_=GL_COMPRESSED_RGB;
                break;
            }
            case 4:
            {
                image_data_format = GL_RGBA;
                texture2d->gl_texture_format_=GL_COMPRESSED_RGBA;
                break;
            }
        }
    }


    //1. 通知显卡创建纹理对象，返回句柄;
    glGenTextures(1, &(texture2d->gl_texture_id_));

    //2. 将纹理绑定到特定纹理目标;
    glBindTexture(GL_TEXTURE_2D, texture2d->gl_texture_id_);

    stopwatch.restart();

    //3. 将图片rgb数据上传到GPU;并进行压缩。
    glTexImage2D(GL_TEXTURE_2D, texture2d->mipmap_level_, texture2d->gl_texture_format_, texture2d->width_, texture2d->height_, 0, image_data_format, GL_UNSIGNED_BYTE, data);
    stopwatch.stop();
    std::int64_t upload_and_compress_cost = stopwatch.milliseconds();

    //4. 指定放大，缩小滤波方式，线性滤波，即放大缩小的插值方式;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //释放图片文件内存
    stbi_image_free(data);

    return texture2d;
}

void Texture2D::CompressImageFile(std::string& image_file_path,std::string& save_image_file_path) {

    Texture2D* texture2d=LoadFromFile(image_file_path);

    //1. 获取压缩是否成功
    GLint compress_success=0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compress_success);

    //2. 获取压缩好的纹理数据尺寸
    GLint compress_size=0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compress_size);

    //3. 获取具体的纹理压缩格式
    GLint compress_format=0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &compress_format);

    //4. 从GPU中，将显存中保存的压缩好的纹理数据，下载到内存
    void* img=malloc(compress_size);
    glGetCompressedTexImage(GL_TEXTURE_2D,0,img);

    //5. 保存为文件
    ofstream output_file_stream(save_image_file_path,ios::out | ios::binary);

    CptFileHead cpt_file_head;
    cpt_file_head.type_[0]='c';
    cpt_file_head.type_[1]='p';
    cpt_file_head.type_[2]='t';
    cpt_file_head.mipmap_level_=texture2d->mipmap_level_;
    cpt_file_head.width_=texture2d->width_;
    cpt_file_head.height_=texture2d->height_;
    cpt_file_head.gl_texture_format_=compress_format;
    cpt_file_head.compress_size_=compress_size;

    output_file_stream.write((char*)&cpt_file_head, sizeof(CptFileHead));
    output_file_stream.write((char*)img,compress_size);
    output_file_stream.close();
}
