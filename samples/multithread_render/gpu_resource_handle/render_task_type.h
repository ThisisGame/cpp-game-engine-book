//
// Created by captainchen on 2022/3/3.
//

#ifndef UNTITLED_RENDER_TASK_TYPE_H
#define UNTITLED_RENDER_TASK_TYPE_H

#include <thread>
#include "render_command.h"

/// 渲染任务基类
class RenderTaskBase{
public:
    RenderTaskBase(){}
    virtual ~RenderTaskBase(){}
public:
    RenderCommand render_command_;//渲染命令
    bool need_return_result = false;//是否需要回传结果
    bool return_result_set = false;//是否设置好了回传结果
};

/// 需要回传结果的阻塞性任务
class RenderTaskNeedReturnResult: public RenderTaskBase{
public:
    RenderTaskNeedReturnResult(){
        render_command_=RenderCommand::NONE;
        need_return_result=true;
    }
    ~RenderTaskNeedReturnResult(){}
    /// 等待任务在渲染线程执行完毕，并设置回传结果。主线程拿到结果后才能执行下一步代码。
    virtual void Wait(){
        while(return_result_set==false){
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
};


/// 编译着色器任务
class RenderTaskCompileShader: public RenderTaskBase{
public:
    RenderTaskCompileShader(){
        render_command_=RenderCommand::COMPILE_SHADER;
    }
    ~RenderTaskCompileShader(){}
public:
    const char* vertex_shader_source_= nullptr;
    const char* fragment_shader_source_= nullptr;
    unsigned int shader_program_handle_= 0;
};

/// 创建VAO任务
class RenderTaskCreateVAO: public RenderTaskBase{
public:
    RenderTaskCreateVAO(){
        render_command_=RenderCommand::CREATE_VAO;
    }
    ~RenderTaskCreateVAO(){}
public:
    unsigned int shader_program_handle_=0;//着色器程序句柄
    const void* positions_=nullptr;//顶点位置
    GLsizei   positions_stride_=0;//顶点数据大小
    const void* colors_=nullptr;//顶点颜色
    GLsizei   colors_stride_=0;//颜色数据大小
    unsigned int vao_handle_=0;//VAO句柄
};

/// 绘制任务
class RenderTaskDrawArray: public RenderTaskBase {
public:
    RenderTaskDrawArray(){
        render_command_=RenderCommand::DRAW_ARRAY;
    }
    ~RenderTaskDrawArray(){}
public:
    GLuint shader_program_handle_=0;//着色器程序句柄
    GLuint vao_handle_=0;//VAO句柄
};

/// 特殊任务：帧结束标志，渲染线程收到这个任务后，刷新缓冲区，设置帧结束。
class RenderTaskEndFrame: public RenderTaskNeedReturnResult {
public:
    RenderTaskEndFrame(){
        render_command_=RenderCommand::END_FRAME;
    }
    ~RenderTaskEndFrame(){}
};


#endif //UNTITLED_RENDER_TASK_TYPE_H
