//
// Created by captainchen on 2022/6/7.
//

#include "environment.h"
#include "component/game_object.h"
#include "renderer/mesh_renderer.h"
#include "renderer/material.h"
#include "render_device/gpu_resource_mapper.h"
#include "render_device/render_task_producer.h"

Environment::Environment():
    ambient_color_(glm::vec3(1.0f, 1.0f, 1.0f)),ambient_color_intensity_(1.0f),uniform_buffer_object_handle_(0) {
}

void Environment::Update() {
    if(uniform_buffer_object_handle_==0){
        uniform_buffer_object_handle_=GPUResourceMapper::GenerateVBOHandle();
        //发出任务：创建UBO
        RenderTaskProducer::ProduceRenderTaskCreateVAO(shader_program_handle, vertex_array_object_handle_,vertex_buffer_object_handle_, mesh->vertex_num_ * sizeof(MeshFilter::Vertex), sizeof(MeshFilter::Vertex), mesh->vertex_data_, mesh->vertex_index_num_ * sizeof(unsigned short), mesh->vertex_index_data_);
    }
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