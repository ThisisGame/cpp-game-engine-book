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

/// 键盘回调
/// \param window
/// \param key
/// \param scancode
/// \param action
/// \param mods
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::RecordKey(key,action);
}
/// 鼠标按键回调
/// \param window
/// \param button
/// \param action
/// \param mods
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Input::RecordKey(button,action);
//    std::cout<<"mouse_button_callback:"<<button<<","<<action<<std::endl;
}
/// 鼠标移动回调
/// \param window
/// \param x
/// \param y
static void mouse_move_callback(GLFWwindow* window, double x, double y)
{
    Input::set_mousePosition(x,y);
//    std::cout<<"mouse_move_callback:"<<x<<","<<y<<std::endl;
}
/// 鼠标滚轮回调
/// \param window
/// \param x
/// \param y
static void mouse_scroll_callback(GLFWwindow* window, double x, double y)
{
    Input::RecordScroll(y);
//    std::cout<<"mouse_scroll_callback:"<<x<<","<<y<<std::endl;
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
    glfwSetMouseButtonCallback(glfw_window_,mouse_button_callback);
    glfwSetScrollCallback(glfw_window_,mouse_scroll_callback);
    glfwSetCursorPosCallback(glfw_window_,mouse_move_callback);
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
