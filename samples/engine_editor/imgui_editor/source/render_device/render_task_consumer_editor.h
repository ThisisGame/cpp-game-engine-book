//
// Created by captainchen on 2022/2/7.
//

#ifndef UNTITLED_RENDER_TASK_CONSUMER_EDITOR_H
#define UNTITLED_RENDER_TASK_CONSUMER_EDITOR_H

#include <thread>
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

private:
    GLFWwindow* window_=nullptr;
};



#endif //UNTITLED_RENDER_TASK_CONSUMER_EDITOR_H
