//
// Created by captainchen on 2021/10/1.
//

#ifndef UNTITLED_FONT_H
#define UNTITLED_FONT_H

#include <iostream>
#include <unordered_map>
#include "freetype/ftglyph.h"

class Texture2D;
class Font {
public:
    Texture2D* font_texture(){return font_texture_;}

    /// freetype为字符生成bitmap
    /// \param c
    void LoadCharacter(char ch);

    /// 为字符串生成bitmap
    /// \param str
    void LoadStr(std::string str);

private:
    unsigned short font_size_=20;//默认字体大小
    char* font_file_buffer_= nullptr;//ttf字体文件加载到内存
    FT_Library ft_library_;
    FT_Face ft_face_;
    Texture2D* font_texture_;
    unsigned short font_texture_size_=1024;
    unsigned short font_texture_fill_x=0;//
    unsigned short font_texture_fill_y=0;

public:
    /// 加载一个字体文件并解析
    /// \param image_file_path ttf字体文件路径
    /// \param font_size 默认文字尺寸
    /// \return
    static Font* LoadFromFile(std::string font_file_path,unsigned short font_size);

    /// 获取Font实例
    /// \param font_file_path ttf路径
    /// \return
    static Font* GetFont(std::string font_file_path);
private:
    static std::unordered_map<std::string,Font*> font_map_;//存储加载的字体 key：ttf路径 value：Font实例
};


#endif //UNTITLED_FONT_H
