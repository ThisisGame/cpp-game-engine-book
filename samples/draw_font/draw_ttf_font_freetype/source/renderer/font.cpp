//
// Created by captainchen on 2021/10/1.
//

#include "font.h"
#include <fstream>
#include "../utils/application.h"
#include "spdlog/spdlog.h"
#include "texture2d.h"

using std::ifstream;
using std::ios;

Font* Font::LoadFromFile(std::string font_file_path,unsigned short font_size){
    Font* font=GetFont(font_file_path);
    if(font!= nullptr){
        return font;
    }

    //读取 ttf 字体文件
    ifstream input_file_stream(Application::data_path()+ font_file_path,ios::in | ios::binary);
    input_file_stream.seekg(0,std::ios::end);
    int len = input_file_stream.tellg();
    input_file_stream.seekg(0,std::ios::beg);
    char *font_file_buffer = new char[len];
    input_file_stream.read(font_file_buffer , len);

    //将ttf 传入FreeType解析
    FT_Library ft_library= nullptr;
    FT_Face ft_face= nullptr;
    FT_Error error = FT_New_Memory_Face(ft_library, (const FT_Byte*)font_file_buffer, len, 0, &ft_face);
    if (error != 0){
        spdlog::error("FT_New_Memory_Face return error {}!",error);
        return nullptr;
    }

    FT_Select_Charmap(ft_face, FT_ENCODING_UNICODE);

    FT_F26Dot6 ft_size = (FT_F26Dot6)(font_size*(1 << 6));

    FT_Set_Char_Size(ft_face, ft_size, 0, 72, 72);

    if (ft_face == nullptr){
        spdlog::error("FT_Set_Char_Size error!");
        return nullptr;
    }
    
    font=new Font();
    font->font_size_=font_size;
    font->font_file_buffer_=font_file_buffer;

    //创建空白的、仅Alpha通道纹理，用于生成文字。
    unsigned char * pixels = (unsigned char *)malloc(1024 * 1024);
    memset(pixels, 0,1024*1024);
    font->font_texture_=Texture2D::Create(1024,1024,GL_ALPHA,GL_UNSIGNED_BYTE,pixels);

    return font;
}

Font* Font::GetFont(std::string font_file_path) {
    return font_map_[font_file_path];
}