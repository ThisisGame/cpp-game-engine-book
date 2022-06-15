//
// Created by captainchen on 2022/6/7.
//

#include "environment.h"
#include "component/game_object.h"
#include "renderer/mesh_renderer.h"
#include "renderer/material.h"

Environment::Environment():
    ambient_color_(glm::vec3(1.0f, 1.0f, 1.0f)),ambient_color_intensity_(1.0f) {
}

void Environment::Update() {
    //遍历所有相机，每个相机的View Projection，都用来做一次渲染。
    GameObject::Foreach([this](GameObject* game_object){
        if(game_object->active()==false){
            return;
        }
        MeshRenderer* mesh_renderer=game_object->GetComponent<MeshRenderer>();
        if(mesh_renderer== nullptr){
            return;
        }
        Material* material = mesh_renderer->material();
        material->SetUniform3f("u_ambient.light_color",ambient_color_);
        material->SetUniform1f("u_ambient.light_intensity",ambient_color_intensity_);
    });
}