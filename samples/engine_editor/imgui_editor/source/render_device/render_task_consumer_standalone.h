//
// Created by captainchen on 2022/2/7.
//

#ifndef UNTITLED_RENDER_TASK_CONSUMER_STANDALONE_H
#define UNTITLED_RENDER_TASK_CONSUMER_STANDALONE_H

#include <thread>
#include "render_task_consumer_base.h"

class GLFWwindow;
class RenderTaskBase;

/// 渲染任务消费端(PC系统游戏程序)
class RenderTaskConsumerStandalone : public RenderTaskConsumerBase{
public:
    RenderTaskConsumerStandalone(GLFWwindow* window);
    ~RenderTaskConsumerStandalone();

    virtual void InitGraphicsLibraryFramework() override;

    virtual void GetFramebufferSize(int& width,int& height) override;

    virtual void SwapBuffer() override;

private:
    GLFWwindow* window_=nullptr;
};



#endif //UNTITLED_RENDER_TASK_CONSUMER_STANDALONE_H
