//
// Created by captain on 2021/4/6.
// 图片加载与解析 https://www.linuxidc.com/linux/2015-02/114021.htm https://docs.gl/gl4/glCompressedTexImage2D
//
#include "texture2d.h"
#include <fstream>
#include "timetool/stopwatch.h"
#include "../utils/application.h"

using std::ifstream;
using std::ios;
using timetool::StopWatch;

Texture2D* Texture2D::LoadFromFile(std::string& image_file_path)
{
    Texture2D* texture2d=new Texture2D();

    StopWatch stopwatch;
    stopwatch.start();
    //读取 cpt 压缩纹理文件
    ifstream input_file_stream(Application::data_path()+ image_file_path,ios::in | ios::binary);
    CptFileHead cpt_file_head;
    input_file_stream.read((char*)&cpt_file_head, sizeof(CptFileHead));

    unsigned char* data =(unsigned char*)malloc(cpt_file_head.compress_size_);
    input_file_stream.read((char*)data, cpt_file_head.compress_size_);
    input_file_stream.close();
    stopwatch.stop();
    std::int64_t load_cpt_cost = stopwatch.milliseconds();

    texture2d->gl_texture_format_=cpt_file_head.gl_texture_format_;
    texture2d->width_=cpt_file_head.width_;
    texture2d->height_=cpt_file_head.height_;


    //1. 通知显卡创建纹理对象，返回句柄;
    glGenTextures(1, &(texture2d->gl_texture_id_));

    //2. 将纹理绑定到特定纹理目标;
    glBindTexture(GL_TEXTURE_2D, texture2d->gl_texture_id_);

    stopwatch.restart();
    {
        //3. 将压缩纹理数据上传到GPU;
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, texture2d->gl_texture_format_, texture2d->width_, texture2d->height_, 0, cpt_file_head.compress_size_, data);
    }
    stopwatch.stop();
    std::int64_t upload_cpt_cost = stopwatch.milliseconds();

    //4. 指定放大，缩小滤波方式，线性滤波，即放大缩小的插值方式;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    delete (data);

    return texture2d;
}
