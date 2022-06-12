//
// Created by captain on 2021/5/3.
//

#include "material.h"
#include <iostream>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "shader.h"
#include "texture_2d.h"
#include "app/application.h"

using std::ifstream;
using std::ios;
using std::cout;
using std::endl;

Material::Material() = default;

Material::~Material() = default;

void Material::Parse(const string& material_path) {
    //解析xml
    rapidxml::file<> xml_file((Application::data_path()+material_path).c_str());
    rapidxml::xml_document<> document;
    document.parse<0>(xml_file.data());

    //根节点
    rapidxml::xml_node<>* material_node=document.first_node("material");
    if(material_node == nullptr){
        return;
    }

    rapidxml::xml_attribute<>* material_shader_attribute=material_node->first_attribute("shader");
    if(material_shader_attribute == nullptr){
        return;
    }
    shader_=Shader::Find(material_shader_attribute->value());

    //解析Texture
    rapidxml::xml_node<>* material_texture_node=material_node->first_node("texture");
    while (material_texture_node != nullptr){
        rapidxml::xml_attribute<>* texture_name_attribute=material_texture_node->first_attribute("name");
        if(texture_name_attribute == nullptr){
            return;
        }

        rapidxml::xml_attribute<>* texture_image_attribute=material_texture_node->first_attribute("image");
        if(texture_image_attribute == nullptr){
            return;
        }

        std::string shader_property_name=texture_name_attribute->value();
        std::string image_path=texture_image_attribute->value();
        textures_.emplace_back(shader_property_name, image_path.empty()? nullptr:Texture2D::LoadFromFile(image_path));

        material_texture_node=material_texture_node->next_sibling("texture");
    }
}


void Material::SetUniform1i(const std::string& shader_property_name, int value) {
    uniform_1i_map_[shader_property_name]= value;
}

void Material::SetUniform1f(const std::string& shader_property_name, float value) {
    uniform_1f_map_[shader_property_name]=  value;
}

void Material::SetUniform3f(const std::string& shader_property_name,glm::vec3& value){
    uniform_3f_map_[shader_property_name]= value;
}

void Material::SetTexture(const string& property, Texture2D *texture2D) {
    for (auto& pair : textures_){
        if(pair.first==property){
            delete(pair.second);
            pair.second=texture2D;
            break;
    }
    }
}




