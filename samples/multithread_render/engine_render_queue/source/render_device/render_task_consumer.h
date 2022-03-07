//
// Created by captainchen on 2022/2/7.
//

#ifndef UNTITLED_RENDER_TASK_CONSUMER_H
#define UNTITLED_RENDER_TASK_CONSUMER_H

#include <thread>
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spscqueue/include/rigtorp/SPSCQueue.h>
#include <glm/glm.hpp>

class GLFWwindow;
class RenderTaskBase;

/// 渲染任务消费端
class RenderTaskConsumer {
public:
    static void Init(GLFWwindow* window);

    static void Exit();

    /// 添加任务到队列
    /// \param render_task
    static void PushRenderTask(RenderTaskBase* render_task);

private:
    /// 线程主函数：死循环处理渲染任务
    static void ProcessTask();

    /// 更新游戏画面尺寸
    /// \param task_base
    static void UpdateScreenSize(RenderTaskBase* task_base);

    /// 编译、链接Shader
    /// \param task_base
    static void CompileShader(RenderTaskBase *task_base);

    /// 使用Shader程序
    static void UseShaderProgram(RenderTaskBase *task_base);

    /// 创建压缩纹理
    /// \param task_base
    static void CreateCompressedTexImage2D(RenderTaskBase *task_base);

    /// 创建普通纹理
    /// \param task_base
    static void CreateTexImage2D(RenderTaskBase *task_base);


    /// 删除Textures
    /// \param task_base
    static void DeleteTextures(RenderTaskBase *task_base);

    /// 局部更新纹理
    /// \param task_base
    static void UpdateTextureSubImage2D(RenderTaskBase *task_base);

    /// 创建VAO
    /// \param task_base
    static void CreateVAO(RenderTaskBase* task_base);

    /// 更新VBO
    /// \param task_base
    static void UpdateVBOSubData(RenderTaskBase* task_base);

    /// 设置状态,开启或关闭
    /// \param task_base
    static void SetEnableState(RenderTaskBase* task_base);

    /// 设置混合函数
    /// \param task_base
    static void SetBlendingFunc(RenderTaskBase* task_base);

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
    static rigtorp::SPSCQueue<RenderTaskBase*> render_task_queue_;//渲染任务队列
};



#endif //UNTITLED_RENDER_TASK_CONSUMER_H
