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
    /// 从freetype加载一个字符
    /// \param c
    void LoadCharacter(char ch);

    Texture2D* font_texture(){return font_texture_;}

private:
    unsigned short font_size_=20;//默认字体大小
    char* font_file_buffer_= nullptr;//ttf字体文件加载到内存
    FT_Library ft_library_;
    FT_Face ft_face_;
    Texture2D* font_texture_;
    unsigned short font_texture_size_=1024;

public:
    /// 加载一个字体文件并解析
    /// \param image_file_path ttf路径
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
