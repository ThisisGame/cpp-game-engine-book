//
// Created by captainchen on 2021/5/14.
// technique是指一个效果，这个效果包含多个Pass，例如要实现被遮挡透视的效果，第一个Pass改变ZWrite，第二个Pass正常渲染。
//

#ifndef UNTITLED_TECHNIQUE_H
#define UNTITLED_TECHNIQUE_H
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"

class Texture2D;
class Pass;
class Technique {
public:
    Technique();
    ~Technique();

    void Parse(rapidxml::xml_node<>* technique_node);

    std::vector<Pass*>& pass_vec(){return pass_vec_;}

    /// 设置纹理
    /// \param property
    /// \param texture2D
    void SetTexture(const std::string &property, Texture2D* texture2D);
private:
    std::vector<Pass*> pass_vec_;//存储多个pass
};


#endif //UNTITLED_TECHNIQUE_H
