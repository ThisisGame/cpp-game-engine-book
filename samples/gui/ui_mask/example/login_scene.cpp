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
#include "ui/ui_image.h"
#include "ui/ui_mask.h"


using namespace rttr;
RTTR_REGISTRATION
{
    registration::class_<LoginScene>("LoginScene")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

LoginScene::LoginScene() :Component(){}

void LoginScene::Awake() {
    //创建相机1 GameObject
    auto go_camera_1=GameObject::Create("main_camera");
    //挂上 Transform 组件
    transform_camera_1_=dynamic_cast<Transform*>(go_camera_1->AddComponent("Transform"));
    transform_camera_1_->set_position(glm::vec3(0, 0, 10));
    //挂上 Camera 组件
    camera_1_=dynamic_cast<Camera*>(go_camera_1->AddComponent("Camera"));
    camera_1_->set_depth(0);

    last_frame_mouse_position_=Input::mousePosition();

    CreateFishSoupPot();

    CreateFont();

    CreateUI();
}

void LoginScene::CreateFishSoupPot() {
    //创建模型 GameObject
    GameObject* go=GameObject::Create("fishsoup_pot");
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
    std::string str="Captain";
    //生成文字贴图
    Font* font=Font::LoadFromFile("font/hkyuan.ttf",100);
    std::vector<Font::Character*> character_vec=font->LoadStr(str);

    //遍历每个字符进行绘制
    int offset_x=0;
    for(auto character : character_vec){
        offset_x+=2;
        //因为FreeType生成的bitmap是上下颠倒的，所以这里UV坐标也要做对应翻转，将左上角作为零点。
        vector<MeshFilter::Vertex> vertex_vector={
                {{-1.0f+offset_x, 2.0f, 1.0f}, {1.0f,0.0f,0.0f,1.0f},   {character->left_top_x_, character->right_bottom_y_}},
                {{ 1.0f+offset_x, 2.0f, 1.0f}, {1.0f,0.0f,0.0f,1.0f},   {character->right_bottom_x_, character->right_bottom_y_}},
                {{ 1.0f+offset_x,  4.0f, 1.0f}, {0.0f,1.0f,0.0f,1.0f},   {character->right_bottom_x_, character->left_top_y_}},
                {{-1.0f+offset_x,  4.0f, 1.0f}, {0.0f,1.0f,0.0f,1.0f},   {character->left_top_x_, character->left_top_y_}}
        };
        vector<unsigned short> index_vector={
                0,1,2,
                0,2,3
        };
        //创建模型 GameObject
        auto go=GameObject::Create("quad_draw_font_color");
        go->set_layer(0x01);

        //挂上 Transform 组件
        auto transform=dynamic_cast<Transform*>(go->AddComponent("Transform"));
        transform->set_position({-8.f,0.f,0.f});

        //挂上 MeshFilter 组件
        auto mesh_filter=dynamic_cast<MeshFilter*>(go->AddComponent("MeshFilter"));
        mesh_filter->CreateMesh(vertex_vector,index_vector);

        //创建 Material
        auto material=new Material();//设置材质
        material->Parse("material/quad_draw_font_color.mat");

        //挂上 MeshRenderer 组件
        auto mesh_renderer=dynamic_cast<MeshRenderer*>(go->AddComponent("MeshRenderer"));
        mesh_renderer->SetMaterial(material);

        //使用文字贴图
        material->SetTexture("u_diffuse_texture", font->font_texture());
    }
}

void LoginScene::CreateUI() {
    //创建UI相机 GameObject
    auto go_camera_ui=GameObject::Create("ui_camera");
    //挂上 Transform 组件
    auto transform_camera_ui=dynamic_cast<Transform*>(go_camera_ui->AddComponent("Transform"));
    transform_camera_ui->set_position(glm::vec3(0, 0, 10));
    //挂上 Camera 组件
    auto camera_ui=dynamic_cast<Camera*>(go_camera_ui->AddComponent("Camera"));
    camera_ui->set_depth(1);
    camera_ui->set_culling_mask(0x02);
    //UI相机不能清除之前的颜色。不然用第一个相机矩阵渲染的物体就被清除 没了。
    camera_ui->set_clear_flag(GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    //设置正交相机
    camera_ui->SetView(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera_ui->SetOrthographic(-Screen::width()/2,Screen::width()/2,-Screen::height()/2,Screen::height()/2,-100,100);

    //创建 GameObject
    auto go_ui_image=GameObject::Create("image_mod_bag");
    go_ui_image->set_layer(0x02);
    //挂上 Transform 组件
    go_ui_image->AddComponent("Transform");
    //挂上 UIImage 组件
    auto ui_image_mod_bag=dynamic_cast<UIImage*>(go_ui_image->AddComponent("UIImage"));
    ui_image_mod_bag->set_texture(Texture2D::LoadFromFile("images/mod_bag.cpt"));

    //创建 GameObject
    auto go_ui_mask=GameObject::Create("mask_mod_bag");
    go_ui_mask->set_layer(0x02);
    go_ui_mask->SetParent(go_ui_image);
    //挂上 Transform 组件
    auto transform_ui_mask=dynamic_cast<Transform*>(go_ui_mask->AddComponent("Transform"));
//    transform_ui_mask->set_position(glm::vec3(-50.f,0.f,0.f));
    //挂上 UIMask 组件
    auto ui_mask_mod_bag=dynamic_cast<UIMask*>(go_ui_mask->AddComponent("UIMask"));
    ui_mask_mod_bag->set_texture(Texture2D::LoadFromFile("images/mod_bag_mask.cpt"));
}

void LoginScene::Update() {
    camera_1_->SetView(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera_1_->SetPerspective(60.f, Screen::aspect_ratio(), 1.f, 1000.f);

    if(Input::GetKeyUp(KEY_CODE_A)){
        auto go_ui_mask = GameObject::Find("mask_mod_bag");
        go_ui_mask->set_active(!go_ui_mask->active());
    }

    //旋转物体
    if(Input::GetKeyDown(KEY_CODE_R)){
        static float rotate_eulerAngle=0.f;
        rotate_eulerAngle+=0.1f;
        glm::vec3 rotation=transform_fishsoup_pot_->rotation();
        rotation.y=rotate_eulerAngle;
        transform_fishsoup_pot_->set_rotation(rotation);
    }

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
