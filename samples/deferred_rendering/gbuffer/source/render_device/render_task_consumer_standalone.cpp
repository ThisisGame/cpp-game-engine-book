//
// Created by captainchen on 2022/2/7.
//

#include "render_task_consumer_standalone.h"
#include <iostream>
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
#include <glm/gtc/matrix_transform.hpp>
#include "timetool/stopwatch.h"
#include "utils/debug.h"
#include "render_task_type.h"
#include "render_command.h"
#include "gpu_resource_mapper.h"
#include "render_task_queue.h"
#include "utils/screen.h"
#include "render_device/uniform_buffer_object_manager.h"

RenderTaskConsumerStandalone::RenderTaskConsumerStandalone(GLFWwindow* window):RenderTaskConsumerBase(),window_(window) {

}

RenderTaskConsumerStandalone::~RenderTaskConsumerStandalone() {}


void RenderTaskConsumerStandalone::InitGraphicsLibraryFramework() {
    //渲染相关的API调用需要放到渲染线程中。
    glfwMakeContextCurrent(window_);
    gladLoadGL(glfwGetProcAddress);
//    glfwSwapInterval(1);
}

void RenderTaskConsumerStandalone::Exit() {
    RenderTaskConsumerBase::Exit();

    glfwMakeContextCurrent(nullptr);
    glfwDestroyWindow(window_);
    window_=nullptr;
}

void RenderTaskConsumerStandalone::GetFramebufferSize(int& width,int& height) {
    glfwGetFramebufferSize(window_, &width, &height);
}

void RenderTaskConsumerStandalone::SwapBuffer() {
    glfwSwapBuffers(window_);
}