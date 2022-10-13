//
// Created by captainchen on 2022/2/7.
//

#include "render_task_consumer_editor.h"
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
#include "render_task_type.h"
#include "render_command.h"
#include "gpu_resource_mapper.h"
#include "utils/screen.h"
#include "read_pixels_queue.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "glfw-3.3-3.4/deps/stb_image_write.h"

RenderTaskConsumerEditor::RenderTaskConsumerEditor(GLFWwindow* window):RenderTaskConsumerBase(),window_(window) {

}

RenderTaskConsumerEditor::~RenderTaskConsumerEditor() {}


void RenderTaskConsumerEditor::InitGraphicsLibraryFramework() {
    //渲染相关的API调用需要放到渲染线程中。
    glfwMakeContextCurrent(window_);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
    //创建全局FBO，将整个游戏渲染到FBO，提供给编辑器，作为Game视图显示
}

void RenderTaskConsumerEditor::GetFramebufferSize(int& width,int& height) {
    glfwGetFramebufferSize(window_, &width, &height);
}

void RenderTaskConsumerEditor::SwapBuffer() {
    glfwSwapBuffers(window_);

    //复制前缓冲区
    char* buffer;
    int width=Screen::width();
    int height=Screen::height();
    buffer = (char*)calloc(4, width * height);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    //存储到队列
    ReadPixelsBuffer* read_pixels_buffer=new ReadPixelsBuffer(buffer);
    ReadPixelsQueue::Push(read_pixels_buffer);
//    stbi_write_png("offscreen.png",width, height, 4,buffer + (width * 4 * (height - 1)),-width * 4);
}