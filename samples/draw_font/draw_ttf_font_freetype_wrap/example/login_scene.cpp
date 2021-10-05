//
// Created by captain on 2021/7/10.
//

#include "login_scene.h"
#include <rttr/registration>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext.hpp>
#include "utils/application.h"
#include "utils/screen.h"
#include "renderer/camera.h"
#include "renderer/mesh_filter.h"
#include "renderer/material.h"
#include "renderer/mesh_renderer.h"
#include "renderer/texture2d.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "control/key_code.h"
#include "renderer/font.h"


using namespace rttr;
RTTR_REGISTRATION
{
    registration::class_<LoginScene>("LoginScene")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

LoginScene::LoginScene() :Component(){}

void LoginScene::Awake() {
    //创建相机1 GameObject
    auto go_camera_1=new GameObject("main_camera");
    //挂上 Transform 组件
    transform_camera_1_=dynamic_cast<Transform*>(go_camera_1->AddComponent("Transform"));
    transform_camera_1_->set_position(glm::vec3(0, 0, 10));
    //挂上 Camera 组件
    camera_1_=dynamic_cast<Camera*>(go_camera_1->AddComponent("Camera"));
    camera_1_->set_depth(0);

    last_frame_mouse_position_=Input::mousePosition();

//    CreateFishSoupPot();

    CreateFont();
}

void LoginScene::CreateFishSoupPot() {
    //创建模型 GameObject
    GameObject* go=new GameObject("fishsoup_pot");
    go->set_layer(0x01);

    //挂上 Transform 组件
    transform_fishsoup_pot_ =dynamic_cast<Transform*>(go->AddComponent("Transform"));

    //挂上 MeshFilter 组件
    auto mesh_filter=dynamic_cast<MeshFilter*>(go->AddComponent("MeshFilter"));
    mesh_filter->LoadMesh("model/fishsoup_pot.mesh");

    //挂上 MeshRenderer 组件
    auto mesh_renderer=dynamic_cast<MeshRenderer*>(go->AddComponent("MeshRenderer"));
    auto material =new Material();//设置材质
    material->Parse("material/fishsoup_pot.mat");
    mesh_renderer->SetMaterial(material);
}

void LoginScene::CreateFont() {
    std::string str="CaptainChen";
    //生成文字贴图
    Font* font=Font::LoadFromFile("font/hkyuan.ttf",300);
    std::vector<Font::Character*> character_vec=font->LoadStr(str);

    for(auto character : character_vec){
        vector<MeshFilter::Vertex> vertex_vector={
                {{-1.0f, -1.0f, 1.0f}, {1.0f,1.0f,1.0f,1.0f},   {0.0f, 0.0f}},
                {{ 1.0f, -1.0f, 1.0f}, {1.0f,1.0f,1.0f,1.0f},   {1.0f, 0.0f}},
                {{ 1.0f,  1.0f, 1.0f}, {1.0f,1.0f,1.0f,1.0f},   {1.0f, 1.0f}},
                {{-1.0f,  1.0f, 1.0f}, {1.0f,1.0f,1.0f,1.0f},   {0.0f, 1.0f}}
        };
        vector<unsigned short> index_vector={
                0,1,2,
                0,2,3
        };
        //创建模型 GameObject
        auto go=new GameObject("quad_draw_font");
        go->set_layer(0x01);

        //挂上 Transform 组件
        auto transform=dynamic_cast<Transform*>(go->AddComponent("Transform"));
        transform->set_position({2.f,0.f,0.f});

        //挂上 MeshFilter 组件
        auto mesh_filter=dynamic_cast<MeshFilter*>(go->AddComponent("MeshFilter"));
        mesh_filter->CreateMesh(vertex_vector,index_vector);

        //创建 Material
        auto material=new Material();//设置材质
        material->Parse("material/quad_draw_font.mat");

        //挂上 MeshRenderer 组件
        auto mesh_renderer=dynamic_cast<MeshRenderer*>(go->AddComponent("MeshRenderer"));
        mesh_renderer->SetMaterial(material);

        //使用文字贴图
        material->SetTexture("u_diffuse_texture", font->font_texture());
    }
}

void LoginScene::Update() {
    camera_1_->SetView(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera_1_->SetProjection(60.f, Screen::aspect_ratio(), 1.f, 1000.f);

//    //旋转物体
//    if(Input::GetKeyDown(KEY_CODE_R)){
//        static float rotate_eulerAngle=0.f;
//        rotate_eulerAngle+=0.1f;
//        glm::vec3 rotation=transform_fishsoup_pot_->rotation();
//        rotation.y=rotate_eulerAngle;
//        transform_fishsoup_pot_->set_rotation(rotation);
//    }

    //旋转相机
    if(Input::GetKeyDown(KEY_CODE_LEFT_ALT) && Input::GetMouseButtonDown(MOUSE_BUTTON_LEFT)){
        float degrees= Input::mousePosition().x_ - last_frame_mouse_position_.x_;

        glm::mat4 old_mat4=glm::mat4(1.0f);

        glm::mat4 rotate_mat4=glm::rotate(old_mat4,glm::radians(degrees),glm::vec3(0.0f,1.0f,0.0f));//以相机所在坐标系位置，计算用于旋转的矩阵，这里是零点，所以直接用方阵。
        glm::vec4 old_pos=glm::vec4(transform_camera_1_->position(),1.0f);
        glm::vec4 new_pos=rotate_mat4*old_pos;//旋转矩阵 * 原来的坐标 = 相机以零点做旋转。
        std::cout<<glm::to_string(new_pos)<<std::endl;

        transform_camera_1_->set_position(glm::vec3(new_pos));
    }
    last_frame_mouse_position_=Input::mousePosition();

    //鼠标滚轮控制相机远近
    transform_camera_1_->set_position(transform_camera_1_->position() *(10 - Input::mouse_scroll())/10.f);
}
