#define GLFW_INCLUDE_NONE

#include <rttr/registration>
#include <glm/glm.hpp>
#include "utils/application.h"
#include "utils/screen.h"
#include "renderer/camera.h"
#include "renderer/mesh_filter.h"
#include "renderer/material.h"
#include "renderer/mesh_renderer.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "control/input.h"
#include "control/key_code.h"

using namespace std;

class LoginScene:public Component{
public:
    LoginScene();

    void Awake();

    void Update();

private:
    Transform* transform_fishsoup_pot;
};

LoginScene::LoginScene() :Component()
{

}

void LoginScene::Awake() {
    //创建模型 GameObject
    GameObject* go=new GameObject("fishsoup_pot");
    go->set_layer(0x01);

    //挂上 Transform 组件
    transform_fishsoup_pot=dynamic_cast<Transform*>(go->AddComponent("Transform"));

    //挂上 MeshFilter 组件
    auto mesh_filter=dynamic_cast<MeshFilter*>(go->AddComponent("MeshFilter"));
    mesh_filter->LoadMesh("model/fishsoup_pot.mesh");

    //挂上 MeshRenderer 组件
    auto mesh_renderer=dynamic_cast<MeshRenderer*>(go->AddComponent("MeshRenderer"));
    Material* material=new Material();//设置材质
    material->Parse("material/fishsoup_pot.mat");
    mesh_renderer->SetMaterial(material);

    //创建相机2 GameObject
    auto go_camera_2=new GameObject("main_camera");
    //挂上 Transform 组件
    auto transform_camera_2=dynamic_cast<Transform*>(go_camera_2->AddComponent("Transform"));
    transform_camera_2->set_position(glm::vec3(1, 0, 10));
    //挂上 Camera 组件
    auto camera_2=dynamic_cast<Camera*>(go_camera_2->AddComponent("Camera"));
    //第二个相机不能清除之前的颜色。不然用第一个相机矩阵渲染的物体就被清除 没了。
    camera_2->set_clear_flag(GL_DEPTH_BUFFER_BIT);
    camera_2->set_depth(1);
    camera_2->set_culling_mask(0x02);
    camera_2->SetView(glm::vec3(transform_camera_2->position().x, 0, 0), glm::vec3(0, 1, 0));
    camera_2->SetProjection(60.f, Screen::aspect_ratio(), 1.f, 1000.f);

    //创建相机1 GameObject
    auto go_camera_1=new GameObject("main_camera");
    //挂上 Transform 组件
    auto transform_camera_1=dynamic_cast<Transform*>(go_camera_1->AddComponent("Transform"));
    transform_camera_1->set_position(glm::vec3(0, 0, 10));
    //挂上 Camera 组件
    auto camera_1=dynamic_cast<Camera*>(go_camera_1->AddComponent("Camera"));
    camera_1->set_depth(0);
    camera_1->SetView(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera_1->SetProjection(60.f, Screen::aspect_ratio(), 1.f, 1000.f);
}

void LoginScene::Update() {
    if(Input::GetKeyDown(KEY_CODE_R)){
        //旋转物体
        static float rotate_eulerAngle=0.f;
        rotate_eulerAngle+=0.1f;
        glm::vec3 rotation=transform_fishsoup_pot->rotation();
        rotation.y=rotate_eulerAngle;
        transform_fishsoup_pot->set_rotation(rotation);
    }
}



using namespace rttr;
RTTR_REGISTRATION
{
registration::class_<LoginScene>("LoginScene")
.constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

int main(void){
    Application::set_data_path("../data/");
    Application::InitOpengl();

    GameObject* go=new GameObject("LoginSceneGo");
    go->AddComponent("Transform");
    go->AddComponent("LoginScene");

    Application::Run();
}