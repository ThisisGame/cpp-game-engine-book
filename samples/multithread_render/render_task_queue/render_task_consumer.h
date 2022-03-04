//
// Created by captainchen on 2022/2/7.
//

#ifndef UNTITLED_RENDER_TASK_CONSUMER_H
#define UNTITLED_RENDER_TASK_CONSUMER_H

#include <thread>
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class GLFWwindow;
class RenderTaskBase;

/// 渲染任务消费端
class RenderTaskConsumer {
public:
    static void Init(GLFWwindow* window);

    static void Exit();

private:
    /// 线程主函数：死循环处理渲染任务
    static void ProcessTask();

    /// 编译、链接Shader
    /// \param task_base
    static void CompileShader(RenderTaskBase *task_base);

    /// 创建缓冲区
    /// \param task_base
    static void RenderTaskConsumer::CreateBuffer(RenderTaskBase* task_base);

    /// 绘制
    /// \param task_base
    /// \param projection
    /// \param view
    static void DrawArray(RenderTaskBase *task_base, glm::mat4 &projection, glm::mat4 &view);

    /// 结束一帧
    /// \param task_base
    static void EndFrame(RenderTaskBase *task_base);
private:
    static GLFWwindow* window_;
    static std::thread render_thread_;//渲染线程
};



#endif //UNTITLED_RENDER_TASK_CONSUMER_H
