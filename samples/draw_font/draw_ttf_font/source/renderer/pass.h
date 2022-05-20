//
// Created by captainchen on 2021/5/14.
//

#ifndef UNTITLED_PASS_H
#define UNTITLED_PASS_H
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"

class Shader;
class Texture2D;
class Pass {
public:
    Pass();
    ~Pass();

    void Parse(rapidxml::xml_node<>* pass_node);

    Shader* shader(){return shader_;}

    std::vector<std::pair<std::string,Texture2D*>>& textures(){return textures_;}

    /// 设置纹理
    /// \param property
    /// \param texture2D
    void SetTexture(std::string &property, Texture2D* texture2D);
private:
    Shader* shader_;
    std::vector<std::pair<std::string,Texture2D*>> textures_;
};


#endif //UNTITLED_PASS_H
