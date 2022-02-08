//
// Created by captainchen on 2022/2/7.
//

#ifndef UNTITLED_RENDERER_H
#define UNTITLED_RENDERER_H

#include <thread>
#include <glad/gl.h>
#include <spscqueue/include/rigtorp/SPSCQueue.h>
#include <glm/glm.hpp>

/// 渲染命令
enum RenderCommand {
    COMPILE_SHADER,//编译着色器
    DRAW_ARRAY,//绘制
};

//渲染命令参数基类
class RenderCommandParamBase{
public:
    RenderCommandParamBase(){}
    virtual ~RenderCommandParamBase(){}

public:
    bool need_return_result = false;//是否需要返回结果
};

/// 编译着色器任务参数
class RenderCommandParamCompileShader: public RenderCommandParamBase{
public:
    RenderCommandParamCompileShader(){}
    ~RenderCommandParamCompileShader(){}
public:
    const char* vertex_shader_source_= nullptr;
    const char* fragment_shader_source_= nullptr;
public:
    GLuint result_program_id_=0;//存储编译Shader结果的ProgramID
};

/// 绘制任务参数
class RenderCommandParamDrawArray: public RenderCommandParamBase {
public:
    RenderCommandParamDrawArray(){}
    ~RenderCommandParamDrawArray(){}
public:
    GLuint program_id_=0;//着色器ProgramID
    const void* positions_=nullptr;//顶点位置
    GLsizei   positions_stride_=0;//顶点数据大小
    const void* colors_=nullptr;//顶点颜色
    GLsizei   colors_stride_=0;//颜色数据大小
};

/// 渲染任务
class RenderTask{
public:
    RenderTask(){}
    ~RenderTask(){
        if(param_){
            delete param_;
            param_ = nullptr;
        }
    }
public:
    RenderCommand render_command_;//渲染命令
    RenderCommandParamBase* param_= nullptr;//渲染命令所需的参数
};

class GLFWwindow;
class Renderer {
public:
    Renderer(GLFWwindow* window);
    ~Renderer();

    void PushRenderTask(RenderTask* render_task){
        render_task_queue_.push(render_task);
    }

    /// 渲染任务队列是否空，用来判定这一帧渲染完成.
    bool IsQueueEmpty(){
        return render_task_queue_.empty();
    }
private:
    void RenderMain();
private:
    GLFWwindow* window_;
    std::thread render_thread_;//渲染线程
    rigtorp::SPSCQueue<RenderTask*> render_task_queue_;//渲染任务队列
};



#endif //UNTITLED_RENDERER_H
