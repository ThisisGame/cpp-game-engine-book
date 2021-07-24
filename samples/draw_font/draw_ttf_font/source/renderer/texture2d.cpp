//
// Created by captain on 2021/4/6.
// 图片加载与解析 https://www.linuxidc.com/linux/2015-02/114021.htm https://docs.gl/gl4/glCompressedTexImage2D
//
#define STB_TRUETYPE_IMPLEMENTATION
#include "texture2d.h"
#include <fstream>
#include "timetool/stopwatch.h"
#include "../utils/application.h"
#include "stb/stb_truetype.h"
#include "spdlog/spdlog.h"


using std::ifstream;
using std::ios;
using timetool::StopWatch;

Texture2D::Texture2D() :mipmap_level_(0),width_(0),height_(0),gl_texture_format_(0),gl_texture_id_(0)
{

}

Texture2D::~Texture2D() {
    if(gl_texture_id_>0){
        glDeleteTextures(1,&gl_texture_id_);
    }
}

Texture2D* Texture2D::LoadFromFile(std::string image_file_path)
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

Texture2D *Texture2D::CreateFromTrueTypeFont(std::string ttf_file_path, const char* word) {
    //加载ttf字体文件
    FILE* font_file = fopen((Application::data_path()+ ttf_file_path).c_str(), "rb");
    fseek(font_file, 0, SEEK_END);
    long ttf_file_size = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    unsigned char* font_buffer = static_cast<unsigned char *>(malloc(ttf_file_size));

    fread(font_buffer, ttf_file_size, 1, font_file);
    fclose(font_file);

    //初始化stb ttf
    stbtt_fontinfo font_info;
    if (!stbtt_InitFont(&font_info, font_buffer, 0))
    {
        spdlog::error("Texture2D::CreateFromTrueTypeFont stbtt_InitFont failed\n");
        delete (font_buffer);
        return nullptr;
    }

    int bitmap_width = 512; /* bitmap width */
    int bitmap_height = 512; /* bitmap height */
    int line_height = 64; /* line height */

    //创建一张bitmap，用来存储stb创建的文字图像
    unsigned char* bitmap = static_cast<unsigned char *>(calloc(bitmap_width * bitmap_height, sizeof(unsigned char)));

    //根据指定的行高，计算 font_info
    float scale = stbtt_ScaleForPixelHeight(&font_info, line_height);

    int x = 0;

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &lineGap);

    ascent = roundf(ascent * scale);
    descent = roundf(descent * scale);

    for (int i = 0; i < strlen(word); ++i)
    {
        /* how wide is this character */
        int ax;
        int lsb;
        stbtt_GetCodepointHMetrics(&font_info, word[i], &ax, &lsb);

        /* get bounding box for character (may be offset to account for chars that dip above or below the line */
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&font_info, word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

        /* compute y (different characters have different heights */
        int y = ascent + c_y1;

        /* render character (stride and offset is important here) */
        int byteOffset = x + roundf(lsb * scale) + (y * bitmap_width);
        stbtt_MakeCodepointBitmap(&font_info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitmap_width, scale, scale, word[i]);

        /* advance x */
        x += roundf(ax * scale);

        /* add kerning */
        int kern;
        kern = stbtt_GetCodepointKernAdvance(&font_info, word[i], word[i + 1]);
        x += roundf(kern * scale);
    }

    //上下翻转
    for (int i = 0; i < bitmap_width; ++i) {
        for (int j = 0; j < bitmap_height/2; ++j) {
            unsigned char top_char=bitmap[bitmap_width*j+i];
            unsigned char bottom_char=bitmap[bitmap_width*(bitmap_height-j-1)+i];

            bitmap[bitmap_width*(bitmap_height-j-1)+i]=top_char;
            bitmap[bitmap_width*j+i]=bottom_char;
        }
    }

    Texture2D* texture2d=new Texture2D();

    texture2d->gl_texture_format_=GL_RED;
    texture2d->width_=bitmap_width;
    texture2d->height_=bitmap_height;

    //1. 通知显卡创建纹理对象，返回句柄;
    glGenTextures(1, &(texture2d->gl_texture_id_));

    //2. 将纹理绑定到特定纹理目标;
    glBindTexture(GL_TEXTURE_2D, texture2d->gl_texture_id_);

    //3. 将图片rgb数据上传到GPU;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture2d->width_, texture2d->height_, 0, texture2d->gl_texture_format_, GL_UNSIGNED_BYTE, bitmap);

    //4. 指定放大，缩小滤波方式，线性滤波，即放大缩小的插值方式;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    delete (bitmap);
    delete (font_buffer);

    return texture2d;
}


