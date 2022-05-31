//
// Created by captainchen on 2021/5/14.
//

#include "application.h"
#include <memory>
#include <rttr/registration>
#include <easy/profiler.h>
#include <glad/gl.h>
#ifdef WIN32
// 避免出现APIENTRY重定义警告。
// freetype引用了windows.h，里面定义了APIENTRY。
// glfw3.h会判断是否APIENTRY已经定义然后再定义一次。
// 但是从编译顺序来看glfw3.h在freetype之前被引用了，判断不到 Windows.h中的定义，所以会出现重定义。
// 所以在 glfw3.h之前必须引用  Windows.h。
#include <Windows.h>
#endif
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "debug.h"
#include "component/game_object.h"
#include "renderer/camera.h"
#include "renderer/mesh_renderer.h"
#include "control/input.h"
#include "screen.h"
#include "render_device/render_device_opengl.h"
#include "render_device/render_task_consumer.h"
#include "audio/audio.h"
#include "time.h"
#include "render_device/render_task_producer.h"

std::string Application::title_;
std::string Application::data_path_;
GLFWwindow* Application::glfw_window_;

static void error_callback(int error, const char* description)
{
    DEBUG_LOG_ERROR("glfw error:{} description:{}",error,description);
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

void Application::Init() {
    EASY_MAIN_THREAD;
    profiler::startListen();// 启动profiler服务器，等待gui连接。

    Debug::Init();
    DEBUG_LOG_INFO("game start");
    Time::Init();
    RenderDevice::Init(new RenderDeviceOpenGL());
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        DEBUG_LOG_ERROR("glfw init failed!");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfw_window_ = glfwCreateWindow(960, 640, title_.c_str(), NULL, NULL);
    if (!glfw_window_)
    {
        DEBUG_LOG_ERROR("glfwCreateWindow error!");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(glfw_window_, key_callback);
    glfwSetMouseButtonCallback(glfw_window_,mouse_button_callback);
    glfwSetScrollCallback(glfw_window_,mouse_scroll_callback);
    glfwSetCursorPosCallback(glfw_window_,mouse_move_callback);

    //初始化渲染任务消费者(单独渲染线程)
    RenderTaskConsumer::Init(glfw_window_);

    UpdateScreenSize();

    //初始化 fmod
    Audio::Init();
}


void Application::Update(){
    EASY_FUNCTION(profiler::colors::Magenta); // 标记函数
    Time::Update();
    UpdateScreenSize();

    GameObject::Foreach([](GameObject* game_object){
        if(game_object->active()){
            game_object->ForeachComponent([](Component* component){
                component->Update();
            });
        }
    });

    Input::Update();
    Audio::Update();
//    std::cout<<"Application::Update"<<std::endl;
}


void Application::Render(){
    EASY_FUNCTION(profiler::colors::Magenta); // 标记函数
    //遍历所有相机，每个相机的View Projection，都用来做一次渲染。
    Camera::Foreach([&](){
        GameObject::Foreach([](GameObject* game_object){
            if(game_object->active()==false){
                return;
            }
            MeshRenderer* mesh_renderer=game_object->GetComponent<MeshRenderer>();
            if(mesh_renderer== nullptr){
                return;
            }
            mesh_renderer->Render();
        });
    });
}

void Application::Run() {
    while (true) {
        EASY_BLOCK("Frame"){
            if(glfwWindowShouldClose(glfw_window_)){
                break;
            }
            Update();
            Render();

            //发出特殊任务：渲染结束
            RenderTaskProducer::ProduceRenderTaskEndFrame();

            EASY_BLOCK("glfwPollEvents"){
                glfwPollEvents();
            }
            EASY_END_BLOCK;
        }EASY_END_BLOCK;
    }

    RenderTaskConsumer::Exit();

    glfwDestroyWindow(glfw_window_);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void Application::UpdateScreenSize() {
    RenderTaskProducer::ProduceRenderTaskUpdateScreenSize();
}
