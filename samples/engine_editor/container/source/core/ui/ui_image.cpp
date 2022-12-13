//
// Created by captain on 2021/10/14.
//

#include "ui_image.h"
#include <vector>
#include "rttr/registration"
#include "core/component/game_object.h"
#include "core/renderer/texture_2d.h"
#include "core/renderer/material.h"
#include "core/renderer/mesh_renderer.h"
#include "core/render_device/render_task_producer.h"
#include "core/utils/debug.h"

using namespace rttr;
RTTR_REGISTRATION{
    registration::class_<UIImage>("UIImage")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

UIImage::UIImage():Component() {

}

UIImage::~UIImage() {

}

/// 指定图片路径加载并设置
/// \param texture_file_path
void UIImage::LoadTexture2D(const char* texture_file_path){
    Texture2D* texture_2d=Texture2D::LoadFromFile(texture_file_path);
    set_texture(texture_2d);
}

void UIImage::Update() {
    Component::Update();
    if(texture2D_== nullptr){
        return;
    }
    MeshFilter* mesh_filter=game_object()->GetComponent<MeshFilter>();
    if(mesh_filter== nullptr){
        //创建 MeshFilter
        std::vector<MeshFilter::Vertex> vertex_vector={
                { {0.f, 0.0f, 0.0f}, {1.0f,1.0f,1.0f,1.0f},   {0.f, 0.f} },
                { {texture2D_->width(), 0.0f, 0.0f}, {1.0f,1.0f,1.0f,1.0f},   {1.f, 0.f} },
                { {texture2D_->width(),  texture2D_->height(), 0.0f}, {1.0f,1.0f,1.0f,1.0f},   {1.f, 1.f} },
                { {0.f,  texture2D_->height(), 0.0f}, {1.0f,1.0f,1.0f,1.0f},   {0.f, 1.f} }
        };
        std::vector<unsigned short> index_vector={
                0,1,2,
                0,2,3
        };
        mesh_filter=game_object()->AddComponent<MeshFilter>();
        mesh_filter->CreateMesh(vertex_vector,index_vector);

        //创建 Material
        auto material=new Material();//设置材质
        material->Parse("material/ui_image.mat");
        material->SetTexture("u_diffuse_texture", texture2D_);

        //挂上 MeshRenderer 组件
        auto mesh_renderer=game_object()->AddComponent<MeshRenderer>();
        mesh_renderer->SetMaterial(material);
    }
}

void UIImage::OnPreRender() {
    Component::OnPreRender();
    RenderTaskProducer::ProduceRenderTaskSetStencilFunc(GL_EQUAL, 0x1, 0xFF);//等于1 通过测试 ,就是上次绘制的图 的范围 才通过测试。
    RenderTaskProducer::ProduceRenderTaskSetStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);//没有通过测试的，保留原来的，也就是保留上一次的值。
}

void UIImage::OnPostRender() {
    Component::OnPostRender();
    RenderTaskProducer::ProduceRenderTaskSetEnableState(GL_STENCIL_TEST, false);
}

