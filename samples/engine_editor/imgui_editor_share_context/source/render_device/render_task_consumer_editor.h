//
// Created by captainchen on 2022/2/7.
//

#ifndef UNTITLED_RENDER_TASK_CONSUMER_EDITOR_H
#define UNTITLED_RENDER_TASK_CONSUMER_EDITOR_H

#include <thread>
#include <glad/gl.h>
#include "render_task_consumer_base.h"


struct GLFWwindow;
class RenderTaskBase;

/// 渲染任务消费端(编辑器)
class RenderTaskConsumerEditor : public RenderTaskConsumerBase{
public:
    RenderTaskConsumerEditor(GLFWwindow* window);
    ~RenderTaskConsumerEditor();

    virtual void InitGraphicsLibraryFramework() override;

    virtual void GetFramebufferSize(int& width,int& height) override;

    virtual void SwapBuffer() override;

    /// FBO 颜色纹理
    /// \return
    GLuint& color_texture_id(){
        return color_texture_id_;
    }

    /// FBO 深度纹理
    /// \return
    GLuint& depth_texture_id(){
        return depth_texture_id_;
    }
private:
    GLFWwindow* window_=nullptr;
    GLuint color_texture_id_=0;//FBO 颜色纹理
    GLuint depth_texture_id_=0;//FBO 深度纹理
};



#endif //UNTITLED_RENDER_TASK_CONSUMER_EDITOR_H
