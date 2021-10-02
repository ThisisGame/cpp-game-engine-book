//
// Created by captainchen on 2021/10/1.
//

#include "font.h"
#include <fstream>
#include "freetype/ftbitmap.h"
#include "../utils/application.h"
#include "spdlog/spdlog.h"
#include "texture2d.h"

using std::ifstream;
using std::ios;

std::unordered_map<std::string,Font*> Font::font_map_;

void Font::LoadCharacter(char ch) {
    //加载这个字的字形,加载到 m_FTFace上面去;Glyph：字形，图形字符 [glif];
    FT_Load_Glyph(ft_face_, FT_Get_Char_Index(ft_face_, ch), FT_LOAD_DEFAULT);

    //从 FTFace上面读取这个字形  到 ft_glyph 变量;
    FT_Glyph ft_glyph;
    FT_Get_Glyph(ft_face_->glyph, &ft_glyph);
    //渲染为256级灰度图
    FT_Glyph_To_Bitmap(&ft_glyph, ft_render_mode_normal, 0, 1);

    FT_BitmapGlyph ft_bitmap_glyph = (FT_BitmapGlyph)ft_glyph;
    FT_Bitmap& ft_bitmap_src = ft_bitmap_glyph->bitmap;
    font_texture_->UpdateSubImage(0,0,ft_bitmap_src.width,ft_bitmap_src.rows,GL_ALPHA,GL_UNSIGNED_BYTE,ft_bitmap_src.buffer);

//    std::ofstream o("./update.bin");
//    if(o){
//        o.write(reinterpret_cast<const char *>(ft_bitmap_src.buffer), ft_bitmap_src.width * ft_bitmap_src.rows);
//        o.flush();
//        o.close();
//    }
}

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
    FT_Init_FreeType(&ft_library);//FreeType初始化;
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
    font->ft_library_=ft_library;
    font->ft_face_=ft_face;
    font_map_[font_file_path]=font;

    //创建空白的、仅Alpha通道纹理，用于生成文字。
    unsigned char * pixels = (unsigned char *)malloc(font->font_texture_size_ * font->font_texture_size_);
    memset(pixels, 0,font->font_texture_size_*font->font_texture_size_);
    font->font_texture_=Texture2D::Create(font->font_texture_size_,font->font_texture_size_,GL_ALPHA,GL_ALPHA,GL_UNSIGNED_BYTE,pixels);
    delete pixels;

    return font;
}

Font* Font::GetFont(std::string font_file_path) {
    return font_map_[font_file_path];
}