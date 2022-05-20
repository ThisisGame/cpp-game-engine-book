//
// Created by captain on 2021/5/3.
//

#ifndef UNTITLED_MATERIAL_H
#define UNTITLED_MATERIAL_H

#include <string>
#include <vector>
#include <map>


class Shader;
class Texture2D;
class Technique;
class Material {
public:
    Material();
    ~Material();

    void Parse(std::string material_path);//加载Material文件并解析


    void SetUniformMatrix4fv(std::string shader_property_name,float* pointer);
    void SetUniform1i(std::string shader_property_name,int value);

    /// 设置纹理
    /// \param property
    /// \param texture2D
    void SetTexture(std::string property, Texture2D* texture2D);

    Technique* technique_active(){return technique_active_;}

private:
    std::vector<Technique*> technique_vec_;
    Technique* technique_active_;

    std::vector<std::pair<std::string,float*>> uniform_matrix4fv_vec;
    std::vector<std::pair<std::string,int>> uniform_1i_vec;
};


#endif //UNTITLED_MATERIAL_H
