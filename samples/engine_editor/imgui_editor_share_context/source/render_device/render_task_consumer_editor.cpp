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

RenderTaskConsumerEditor::RenderTaskConsumerEditor(GLFWwindow* window):RenderTaskConsumerBase(),window_(window) {

}

RenderTaskConsumerEditor::~RenderTaskConsumerEditor() {}


void RenderTaskConsumerEditor::InitGraphicsLibraryFramework() {
    //渲染相关的API调用需要放到渲染线程中。
    glfwMakeContextCurrent(window_);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    //创建全局FBO，将整个游戏渲染到FBO，提供给编辑器，作为Game视图显示
    GLuint frame_buffer_object_id=0;
    glGenFramebuffers(1, &frame_buffer_object_id);__CHECK_GL_ERROR__
    if(frame_buffer_object_id==0){
        DEBUG_LOG_ERROR("CreateFBO FBO Error!");
        return;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,frame_buffer_object_id);__CHECK_GL_ERROR__

    //创建全局RBO
    GLuint renderer_buffer_object_id=0;
    glGenRenderbuffers(1,&renderer_buffer_object_id);
    glBindRenderbuffer(GL_RENDERBUFFER,renderer_buffer_object_id);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA,960,640);

    if(renderer_buffer_object_id==0){
        DEBUG_LOG_ERROR("CreateFBO RBO Error!");
        return;
    }

    //将RBO附加到FBO上
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,renderer_buffer_object_id);

    //检测帧缓冲区完整性，如果完整的话就开始进行绘制
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);__CHECK_GL_ERROR__
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        DEBUG_LOG_ERROR("BindFBO FBO Error,Status:{} !",status);//36055 = 0x8CD7 GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 附着点没有东西 可以绑一个RBO。
        return;
    }
}

void RenderTaskConsumerEditor::GetFramebufferSize(int& width,int& height) {
    glfwGetFramebufferSize(window_, &width, &height);
}

void RenderTaskConsumerEditor::SwapBuffer() {
    glfwSwapBuffers(window_);
}