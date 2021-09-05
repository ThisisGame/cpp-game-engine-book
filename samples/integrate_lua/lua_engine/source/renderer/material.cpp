//
// Created by captain on 2021/5/3.
//

#include "material.h"
#include <iostream>
#include <fstream>
#include "glad/gl.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "shader.h"
#include "texture2d.h"
#include "../utils/application.h"
#include "technique.h"
#include "pass.h"

using std::ifstream;
using std::ios;
using std::cout;
using std::endl;

Material::Material() {

}

Material::~Material() {

}

void Material::Parse(string material_path) {
    //解析xml
    rapidxml::file<> xml_file((Application::data_path()+material_path).c_str());
    rapidxml::xml_document<> document;
    document.parse<0>(xml_file.data());

    //根节点
    rapidxml::xml_node<>* root_node=document.first_node("material");
    if(root_node== nullptr){
        return;
    }

    //获取 technique 节点
    rapidxml::xml_node<>* technique_node=root_node->first_node("technique");
    while (technique_node!= nullptr){
        Technique* technique=new Technique();
        technique->Parse(technique_node);
        technique_vec_.push_back(technique);

        technique_node=technique_node->next_sibling("technique");
    }

    //LOD系统没有实现，就先设定第1个为默认Technique
    technique_active_=technique_vec_[0];
}



void Material::SetUniformMatrix4fv(std::string shader_property_name, float *pointer) {
    uniform_matrix4fv_vec.push_back(std::make_pair(shader_property_name,pointer));
}

void Material::SetUniform1i(std::string shader_property_name, int value) {
    uniform_1i_vec.push_back(std::make_pair(shader_property_name,value));
}

void Material::SetTexture(string property, Texture2D *texture2D) {
    for (auto& technique : technique_vec_){
        technique->SetTexture(property,texture2D);
    }
}




