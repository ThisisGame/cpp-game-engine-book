//
// Created by captain on 2021/5/3.
//

#ifndef UNTITLED_MATERIAL_H
#define UNTITLED_MATERIAL_H

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>


class Shader;
class Texture2D;
class Material {
public:
    Material();
    ~Material();

    void Parse(const std::string& material_path);//加载Material文件并解析

    Shader* shader(){return shader_;}

    void SetUniform1i(const std::string& shader_property_name,int value);
    void SetUniform1f(const std::string& shader_property_name,float value);
    void SetUniform3f(const std::string& shader_property_name,glm::vec3& value);

    /// 设置纹理
    /// \param property
    /// \param texture2D
    void SetTexture(const std::string& property, Texture2D* texture2D);

    std::vector<std::pair<std::string,Texture2D*>>& textures(){return textures_;}
    std::vector<std::pair<std::string,int>>& uniform_1i_vec(){return uniform_1i_vec_;}
    std::vector<std::pair<std::string,float>>& uniform_1f_vec(){return uniform_1f_vec_;}
    std::vector<std::pair<std::string,glm::vec3>>& uniform_3f_vec(){return uniform_3f_vec_;}

private:
    Shader* shader_{};
    std::vector<std::pair<std::string,Texture2D*>> textures_;

    std::vector<std::pair<std::string,int>> uniform_1i_vec_;
    std::vector<std::pair<std::string,float>> uniform_1f_vec_;
    std::vector<std::pair<std::string,glm::vec3>> uniform_3f_vec_;
};


#endif //UNTITLED_MATERIAL_H
