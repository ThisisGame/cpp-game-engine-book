//
// Created by captain on 2021/4/6.
//
#define STB_TRUETYPE_IMPLEMENTATION
#include "texture2d.h"
#include <fstream>
#include "timetool/stopwatch.h"
#include "stb/stb_truetype.h"
#include "utils/debug.h"
#include "glm/glm.hpp"
#include "utils/application.h"



using std::ifstream;
using std::ios;
using timetool::StopWatch;

Texture2D::Texture2D() :mipmap_level_(0),width_(0),height_(0),gl_texture_format_(0),gl_texture_id_(0)
{

}

Texture2D::~Texture2D() {
    if(gl_texture_id_>0){
        glDeleteTextures(1,&gl_texture_id_);__CHECK_GL_ERROR__
    }
}

void Texture2D::UpdateSubImage(int x, int y, int width, int height, unsigned int client_format, unsigned int data_type,
                               unsigned char *data) {
    if(width<=0 || height<=0){
        return;
    }
    glBindTexture(GL_TEXTURE_2D, gl_texture_id_);__CHECK_GL_ERROR__
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);__CHECK_GL_ERROR__
    glTexSubImage2D(GL_TEXTURE_2D,0,x,y,width,height,client_format,data_type,data);__CHECK_GL_ERROR__
}

Texture2D* Texture2D::LoadFromFile(std::string image_file_path)
{
    if(image_file_path.empty()){
        DEBUG_LOG_ERROR("image_file_path empty");
        return nullptr;
    }
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
    glGenTextures(1, &(texture2d->gl_texture_id_));__CHECK_GL_ERROR__

    //2. 将纹理绑定到特定纹理目标;
    glBindTexture(GL_TEXTURE_2D, texture2d->gl_texture_id_);__CHECK_GL_ERROR__

    stopwatch.restart();
    {
        //3. 将压缩纹理数据上传到GPU;
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, texture2d->gl_texture_format_, texture2d->width_, texture2d->height_, 0, cpt_file_head.compress_size_, data);
        __CHECK_GL_ERROR__
    }
    stopwatch.stop();
    std::int64_t upload_cpt_cost = stopwatch.milliseconds();

    //4. 指定放大，缩小滤波方式，线性滤波，即放大缩小的插值方式;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);__CHECK_GL_ERROR__
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);__CHECK_GL_ERROR__

    delete (data);
    return texture2d;
}

Texture2D *Texture2D::Create(unsigned short width, unsigned short height, unsigned int server_format,unsigned int client_format,
                             unsigned int data_type,unsigned char* data) {
    Texture2D* texture2d=new Texture2D();
    texture2d->gl_texture_format_=server_format;
    texture2d->width_=width;
    texture2d->height_=height;

    //1. 通知显卡创建纹理对象，返回句柄;
    glGenTextures(1, &(texture2d->gl_texture_id_));__CHECK_GL_ERROR__

    //2. 将纹理绑定到特定纹理目标;
    glBindTexture(GL_TEXTURE_2D, texture2d->gl_texture_id_);__CHECK_GL_ERROR__

    //3. 将图片rgb数据上传到GPU;
    glTexImage2D(GL_TEXTURE_2D, 0, texture2d->gl_texture_format_, texture2d->width_, texture2d->height_, 0, client_format, data_type, data);
    __CHECK_GL_ERROR__

    //4. 指定放大，缩小滤波方式，线性滤波，即放大缩小的插值方式;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);__CHECK_GL_ERROR__
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);__CHECK_GL_ERROR__

    return texture2d;
}


