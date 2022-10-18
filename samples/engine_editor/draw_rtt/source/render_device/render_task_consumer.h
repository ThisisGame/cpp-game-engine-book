//
// Created by captainchen on 2022/2/7.
//

#ifndef UNTITLED_RENDER_TASK_CONSUMER_H
#define UNTITLED_RENDER_TASK_CONSUMER_H

#include <thread>

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

    /// 更新游戏画面尺寸
    /// \param task_base
    static void UpdateScreenSize(RenderTaskBase* task_base);

    /// 设置视口大小
    /// \param task_base
    static void SetViewportSize(RenderTaskBase* task_base);

    /// 编译、链接Shader
    /// \param task_base
    static void CompileShader(RenderTaskBase *task_base);

    /// 串联uniform block实例与binding point。
    /// \param task_base
    static void ConnectUniformBlockInstanceAndBindingPoint(RenderTaskBase *task_base);

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

    /// 创建UBO
    /// \param task_base
    static void CreateUBO(RenderTaskBase* task_base);

    /// 更新UBO
    /// \param task_base
    static void UpdateUBOSubData(RenderTaskBase* task_base);

    /// 设置状态,开启或关闭
    /// \param task_base
    static void SetEnableState(RenderTaskBase* task_base);

    /// 设置混合函数
    /// \param task_base
    static void SetBlendFunc(RenderTaskBase* task_base);

    /// 上传uniform矩阵
    /// \param task_base
    static void SetUniformMatrix4fv(RenderTaskBase* task_base);

    /// 激活并绑定纹理
    /// \param task_base
    static void ActiveAndBindTexture(RenderTaskBase* task_base);

    /// 上传1个int值
    /// \param task_base
    static void SetUniform1i(RenderTaskBase* task_base);

    /// 上传1个float值
    /// \param task_base
    static void SetUniform1f(RenderTaskBase* task_base);

    /// 上传1个vec3
    /// \param task_base
    static void SetUniform3f(RenderTaskBase* task_base);

    /// 绑定VAO并绘制
    /// \param task_base
    static void BindVAOAndDrawElements(RenderTaskBase* task_base);

    /// 设置clear_flag并且清除颜色缓冲
    /// \param task_base
    static void SetClearFlagAndClearColorBuffer(RenderTaskBase* task_base);

    /// 设置模板测试函数
    static void SetStencilFunc(RenderTaskBase* task_base);

    /// 设置模板操作
    static void SetStencilOp(RenderTaskBase* task_base);

    /// 设置清除模板缓冲值
    static void SetStencilBufferClearValue(RenderTaskBase* task_base);

    /// 创建FBO任务
    static void CreateFBO(RenderTaskBase* task_base);

    /// 绑定使用FBO任务
    static void BindFBO(RenderTaskBase* task_base);

    /// 取消使用FBO任务
    static void UnBindFBO(RenderTaskBase* task_base);

    /// 删除帧缓冲区对象(FBO)
    static void DeleteFBO(RenderTaskBase* task_base);

    /// 结束一帧
    /// \param task_base
    static void EndFrame(RenderTaskBase *task_base);
private:
    static GLFWwindow* window_;
    static std::thread render_thread_;//渲染线程
};



#endif //UNTITLED_RENDER_TASK_CONSUMER_H
