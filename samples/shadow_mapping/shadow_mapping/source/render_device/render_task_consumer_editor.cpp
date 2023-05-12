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
#include "render_task_producer.h"
#include "render_command.h"
#include "gpu_resource_mapper.h"
#include "utils/screen.h"
#include "utils/debug.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "glfw-3.3-3.4/deps/stb_image_write.h"

RenderTaskConsumerEditor::RenderTaskConsumerEditor(GLFWwindow* window):RenderTaskConsumerBase(),window_(window) {

}

RenderTaskConsumerEditor::~RenderTaskConsumerEditor() {}


void RenderTaskConsumerEditor::InitGraphicsLibraryFramework() {
    //渲染相关的API调用需要放到渲染线程中。
    glfwMakeContextCurrent(window_);
    gladLoadGL(glfwGetProcAddress);
//    glfwSwapInterval(1);

    //创建全局FBO，将整个游戏渲染到FBO，提供给编辑器，作为Game视图显示
    GLuint frame_buffer_object_id=0;
    glGenFramebuffers(1, &frame_buffer_object_id);__CHECK_GL_ERROR__
    if(frame_buffer_object_id==0){
        DEBUG_LOG_ERROR("CreateFBO FBO Error!");
        return;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frame_buffer_object_id);__CHECK_GL_ERROR__

    //创建颜色纹理 Attach到FBO颜色附着点上
    glGenTextures(1, &color_texture_id_);
    glBindTexture(GL_TEXTURE_2D, color_texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 960, 640, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);__CHECK_GL_ERROR__
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_id_, 0);__CHECK_GL_ERROR__

    //创建深度纹理 Attach到FBO深度附着点上
    glGenTextures(1, &depth_texture_id_);
    glBindTexture(GL_TEXTURE_2D, depth_texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 960, 640, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);__CHECK_GL_ERROR__
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id_, 0);__CHECK_GL_ERROR__

    //检测帧缓冲区完整性
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);__CHECK_GL_ERROR__
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        DEBUG_LOG_ERROR("BindFBO FBO Error,Status:{} !",status);//36055 = 0x8CD7 GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 附着点没有东西
        return;
    }

    //压入渲染目标栈
    render_target_stack_.Push(frame_buffer_object_id);
}

void RenderTaskConsumerEditor::GetFramebufferSize(int& width,int& height) {
    glfwGetFramebufferSize(window_, &width, &height);
}

void RenderTaskConsumerEditor::SwapBuffer() {
    glfwSwapBuffers(window_);
}