//
// Created by captainchen on 2021/5/14.
//

#include "pass.h"
#include "glad/gl.h"
#include "shader.h"
#include "texture2d.h"

Pass::Pass() {

}

Pass::~Pass() {

}

void Pass::Parse(rapidxml::xml_node<>* pass_node) {
    rapidxml::xml_attribute<>* pass_name_attribute=pass_node->first_attribute("name");
    if(pass_name_attribute== nullptr){
        return;
    }
    rapidxml::xml_attribute<>* pass_shader_attribute=pass_node->first_attribute("shader");
    if(pass_shader_attribute== nullptr){
        return;
    }
    shader_=Shader::Find(pass_shader_attribute->value());

    //解析Texture
    rapidxml::xml_node<>* pass_texture_node=pass_node->first_node("texture");
    while (pass_texture_node!= nullptr){
        rapidxml::xml_attribute<>* pass_texture_name_attribute=pass_texture_node->first_attribute("name");
        if(pass_texture_name_attribute== nullptr){
            return;
        }

        rapidxml::xml_attribute<>* pass_texture_image_attribute=pass_texture_node->first_attribute("image");
        if(pass_texture_image_attribute== nullptr){
            return;
        }

        std::string shader_property_name=pass_texture_name_attribute->value();
        std::string image_path=pass_texture_image_attribute->value();
        textures_.push_back(std::make_pair(pass_texture_name_attribute->value(),Texture2D::LoadFromFile(image_path)));

        pass_texture_node=pass_texture_node->next_sibling("texture");
    }
}

void Pass::SetTexture(string &property, Texture2D *texture2D) {
    for (auto& pair : textures_){
        if(pair.first==property){
            delete(pair.second);
            pair.second=texture2D;
            break;
        }
    }
}

