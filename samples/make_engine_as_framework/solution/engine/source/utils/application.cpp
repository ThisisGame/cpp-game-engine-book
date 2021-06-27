//
// Created by captainchen on 2021/5/14.
//

#include "application.h"
#include <memory>
#include "component/game_object.h"
#include "renderer/camera.h"
#include "renderer/mesh_renderer.h"
#include "control/input.h"
#include "screen.h"

std::string Application::data_path_;
GLFWwindow* Application::glfw_window_;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
//    std::cout<<key<<"-"<<scancode<<"-"<<action<<"-"<<mods<<std::endl;
    Input::RecordKey(key,action);
}

void Application::InitOpengl() {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    glfw_window_ = glfwCreateWindow(960, 640, "Simple example", NULL, NULL);
    if (!glfw_window_)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(glfw_window_);
    gladLoadGL(glfwGetProcAddress);

    UpdateScreenSize();
    glfwSwapInterval(1);

    glfwSetKeyCallback(glfw_window_, key_callback);
}


void Application::Update(){
    UpdateScreenSize();

    GameObject::Foreach([](GameObject* game_object){
        game_object->ForeachComponent([](Component* component){
           component->Update();
        });
    });

    Input::Update();

//    std::cout<<"Application::Update"<<std::endl;
}


void Application::Render(){
    //遍历所有相机，每个相机的View Projection，都用来做一次渲染。
    Camera::Foreach([&](){
        GameObject::Foreach([](GameObject* game_object){
            auto component=game_object->GetComponent("MeshRenderer");
            if (!component){
                return;
            }
            auto mesh_renderer=dynamic_cast<MeshRenderer*>(component);
            if(!mesh_renderer){
                return;
            }
            mesh_renderer->Render();
        });
    });
}

void Application::Run() {
    while (!glfwWindowShouldClose(glfw_window_))
    {
        Update();
        Render();

        glfwSwapBuffers(glfw_window_);

        glfwPollEvents();
    }

    glfwDestroyWindow(glfw_window_);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void Application::UpdateScreenSize() {
    int width, height;
    glfwGetFramebufferSize(glfw_window_, &width, &height);
    glViewport(0, 0, width, height);
    Screen::set_width_height(width,height);
}
