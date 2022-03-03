//
// Created by captainchen on 2022/3/3.
//

#ifndef UNTITLED_RENDER_TASK_TYPE_H
#define UNTITLED_RENDER_TASK_TYPE_H

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
        while(return_result_set==false){}
    }
};

/// 更新游戏画面尺寸任务
class RenderTaskUpdateScreenSize:public RenderTaskNeedReturnResult{
public:
    RenderTaskUpdateScreenSize(){
        render_command_=RenderCommand::UPDATE_SCREEN_SIZE;
    }
    ~RenderTaskUpdateScreenSize(){}
public:
    int width_;//画面宽度
    int height_;//画面高度
};

/// 编译着色器任务
class RenderTaskCompileShader: public RenderTaskNeedReturnResult{
public:
    RenderTaskCompileShader(){
        render_command_=RenderCommand::COMPILE_SHADER;
    }
    ~RenderTaskCompileShader(){}
public:
    const char* vertex_shader_source_= nullptr;
    const char* fragment_shader_source_= nullptr;
public:
    GLuint result_program_id_=0;//存储编译Shader结果的ProgramID
};

/// 删除Texture任务
class RenderTaskDeleteTextures: public RenderTaskBase{
public:
    RenderTaskDeleteTextures(){
        render_command_=RenderCommand::DELETE_TEXTURES;
    }
    ~RenderTaskDeleteTextures(){
        delete [] texture_ids_;
    }
public:
    GLuint* texture_ids_=nullptr;//存储TextureID的数组
    int texture_count_=0;//Texture数量
};

/// 局部更新Texture任务
class RenderTaskUpdateTextureSubImage2D:public RenderTaskBase{
public:
    RenderTaskUpdateTextureSubImage2D(){
        render_command_=RenderCommand::UPDATE_TEXTURE_SUB_IMAGE2D;
    }
    ~RenderTaskUpdateTextureSubImage2D(){}

public:
    GLuint gl_texture_id_;//纹理ID
    int x_,y_,width_,height_;
    unsigned int client_format_;
    unsigned int data_type_;
    unsigned char *data_;
};

/// 绘制任务
class RenderTaskDrawArray: public RenderTaskBase {
public:
    RenderTaskDrawArray(){
        render_command_=RenderCommand::DRAW_ARRAY;
    }
    ~RenderTaskDrawArray(){}
public:
    GLuint program_id_=0;//着色器ProgramID
    const void* positions_=nullptr;//顶点位置
    GLsizei   positions_stride_=0;//顶点数据大小
    const void* colors_=nullptr;//顶点颜色
    GLsizei   colors_stride_=0;//颜色数据大小
};

/// 特殊任务：帧结束标志，渲染线程收到这个任务后，刷新缓冲区，设置帧结束。
class RenderTaskEndFrame: public RenderTaskNeedReturnResult {
public:
    RenderTaskEndFrame(){
        render_command_=RenderCommand::END_FRAME;
    }
    ~RenderTaskEndFrame(){}
public:
    bool render_thread_frame_end_=false;//渲染线程结束一帧
};


#endif //UNTITLED_RENDER_TASK_TYPE_H
