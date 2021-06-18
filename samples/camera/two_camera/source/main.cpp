#define GLFW_INCLUDE_NONE

#include <stdlib.h>
#include <stdio.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "utils/application.h"
#include "renderer/mesh_filter.h"
#include "renderer/shader.h"
#include "renderer/material.h"
#include "renderer/mesh_renderer.h"
#include "renderer/camera.h"

#include "component/component.h"
#include "component/game_object.h"
#include "component/transform.h"

using namespace std;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

GLFWwindow* window;

//初始化OpenGL
void init_opengl()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(960, 640, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
}

int main(void)
{
    Application::set_data_path("../data/");
    init_opengl();

    //创建模型 GameObject
    std::shared_ptr<GameObject> go=std::make_shared<GameObject>("something");

    //挂上 Transform 组件
    auto transform=static_pointer_cast<Transform>(go->AddComponent("Transform"));

    //挂上 MeshFilter 组件
    auto mesh_filter=static_pointer_cast<MeshFilter>(go->AddComponent("MeshFilter"));
    mesh_filter->LoadMesh("model/fishsoup_pot.mesh");

    //挂上 MeshRenderer 组件
    auto mesh_renderer=static_pointer_cast<MeshRenderer>(go->AddComponent("MeshRenderer"));
    std::shared_ptr<Material> material=std::make_shared<Material>();//设置材质
    material->Parse("material/fishsoup_pot.mat");
    mesh_renderer->SetMaterial(material);

    //创建相机1 GameObject
    auto go_camera_1=std::make_shared<GameObject>("main_camera");
    //挂上 Transform 组件
    auto transform_camera_1=static_pointer_cast<Transform>(go_camera_1->AddComponent("Transform"));
    transform_camera_1->set_position(glm::vec3(0, 0, 10));
    //挂上 Camera 组件
    auto camera_1=static_pointer_cast<Camera>(go_camera_1->AddComponent("Camera"));

    //创建相机2 GameObject
    auto go_camera_2=std::make_shared<GameObject>("main_camera");
    //挂上 Transform 组件
    auto transform_camera_2=static_pointer_cast<Transform>(go_camera_2->AddComponent("Transform"));
    transform_camera_2->set_position(glm::vec3(5, 0, 10));
    //挂上 Camera 组件
    auto camera_2=static_pointer_cast<Camera>(go_camera_2->AddComponent("Camera"));
    //第二个相机不能清除之前的颜色。不然用第一个相机矩阵渲染的物体就被清除 没了。
    camera_2->set_clear_flag(GL_DEPTH_BUFFER_BIT);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        ratio = width / (float) height;

        //设置相机1
        camera_1->SetView(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        camera_1->SetProjection(60.f, ratio, 1.f, 1000.f);

        //设置相机2
        camera_2->SetView(glm::vec3(transform_camera_2->position().x, 0, 0), glm::vec3(0, 1, 0));
        camera_2->SetProjection(60.f, ratio, 1.f, 1000.f);

        //旋转物体
        static float rotate_eulerAngle=0.f;
        rotate_eulerAngle+=0.1f;
        glm::vec3 rotation=transform->rotation();
        rotation.y=rotate_eulerAngle;
        transform->set_rotation(rotation);

        //遍历所有相机，每个相机的View Projection，都用来做一次渲染。
        Camera::Foreach([&](){
            mesh_renderer->Render();
        });

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}