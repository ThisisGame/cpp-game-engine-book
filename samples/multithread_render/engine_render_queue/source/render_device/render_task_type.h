//
// Created by captainchen on 2022/3/3.
//

#ifndef UNTITLED_RENDER_TASK_TYPE_H
#define UNTITLED_RENDER_TASK_TYPE_H

#include <stdlib.h>
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
class RenderTaskUpdateScreenSize:public RenderTaskBase{
public:
    RenderTaskUpdateScreenSize(){
        render_command_=RenderCommand::UPDATE_SCREEN_SIZE;
    }
    ~RenderTaskUpdateScreenSize(){}
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

/// 使用着色器程序任务
class RenderTaskUseShaderProgram: public RenderTaskBase{
public:
    RenderTaskUseShaderProgram(){
        render_command_=RenderCommand::USE_SHADER_PROGRAM;
    }
    ~RenderTaskUseShaderProgram(){}
public:
    unsigned int shader_program_handle_= 0;
};

/// 创建压缩纹理任务
class RenderTaskCreateCompressedTexImage2D: public RenderTaskBase{
public:
    RenderTaskCreateCompressedTexImage2D(){
        render_command_=RenderCommand::CREATE_COMPRESSED_TEX_IMAGE2D;
    }
    ~RenderTaskCreateCompressedTexImage2D(){
        free(data_);
    }
public:
    unsigned int texture_handle_= 0;
    int width_;
    int height_;
    unsigned int texture_format_;
    int compress_size_;
    unsigned char* data_;
};

/// 创建纹理任务
class RenderTaskCreateTexImage2D: public RenderTaskBase{
public:
    RenderTaskCreateTexImage2D(){
        render_command_=RenderCommand::CREATE_TEX_IMAGE2D;
    }
    ~RenderTaskCreateTexImage2D(){
        free(data_);
    }
public:
    unsigned int texture_handle_= 0;
    int width_;
    int height_;
    unsigned int gl_texture_format_;//GL内部纹理格式
    unsigned int client_format_;//客户端纹理格式
    unsigned int data_type_;
    unsigned char* data_;
};

/// 删除Texture任务
class RenderTaskDeleteTextures: public RenderTaskBase{
public:
    RenderTaskDeleteTextures(){
        render_command_=RenderCommand::DELETE_TEXTURES;
    }
    ~RenderTaskDeleteTextures(){
        free(texture_handle_array_);
    }
public:
    unsigned int* texture_handle_array_=nullptr;//存储纹理句柄的数组
    int texture_count_=0;//纹理数量
};

/// 局部更新Texture任务
class RenderTaskUpdateTextureSubImage2D:public RenderTaskBase{
public:
    RenderTaskUpdateTextureSubImage2D(){
        render_command_=RenderCommand::UPDATE_TEXTURE_SUB_IMAGE2D;
    }
    ~RenderTaskUpdateTextureSubImage2D(){
        free(data_);
    }

public:
    unsigned int texture_handle_;//纹理句柄
    int x_,y_,width_,height_;
    unsigned int client_format_;
    unsigned int data_type_;
    unsigned char* data_;
    unsigned int data_size_;
};

/// 创建VAO任务
class RenderTaskCreateVAO: public RenderTaskBase{
public:
    RenderTaskCreateVAO(){
        render_command_=RenderCommand::CREATE_VAO;
    }
    ~RenderTaskCreateVAO(){
        free(vertex_data_);
        free(vertex_index_data_);
    }
public:
    unsigned int shader_program_handle_=0;//着色器程序句柄
    unsigned int vao_handle_=0;//VAO句柄
    unsigned int vbo_handle_=0;//VBO句柄
    unsigned int vertex_data_size_;//顶点数据大小
    unsigned int vertex_data_stride_;
    void* vertex_data_;//顶点数据
    unsigned int vertex_index_data_size_;//顶点索引数据大小
    void* vertex_index_data_;//顶点索引数据
};

/// 更新VBO数据
class RenderTaskUpdateVBOSubData:public RenderTaskBase{
public:
    RenderTaskUpdateVBOSubData(){
        render_command_=RenderCommand::UPDATE_VBO_SUB_DATA;
    }
    ~RenderTaskUpdateVBOSubData(){
        free(vertex_data_);
    }
public:
    unsigned int vbo_handle_=0;//VBO句柄
    unsigned int vertex_data_size_;//顶点数据大小
    void* vertex_data_;//顶点数据
};

/// 设置状态，开启或关闭
class RenderTaskSetEnableState: public RenderTaskBase{
public:
    RenderTaskSetEnableState(){
        render_command_=RenderCommand::SET_ENABLE_STATE;
    }
    ~RenderTaskSetEnableState(){}
public:
    unsigned int state_;//OpenGL状态
    bool enable_;//OpenGL状态值
};

/// 设置混合函数
class RenderTaskSetBlenderFunc: public RenderTaskBase{
public:
    RenderTaskSetBlenderFunc(){
        render_command_=RenderCommand::SET_BLENDER_FUNC;
    }
    ~RenderTaskSetBlenderFunc(){}
public:
    unsigned int source_blending_factor_;//源混合因子
    unsigned int destination_blending_factor_;//目标混合因子
};

/// 上传4x4矩阵
class RenderTaskSetUniformMatrix4fv: public RenderTaskBase{
public:
    RenderTaskSetUniformMatrix4fv(){
        render_command_=RenderCommand::SET_UNIFORM_MATRIX_4FV;
    }
    ~RenderTaskSetUniformMatrix4fv(){
        free(uniform_name_);
        free(matrix_data_);
    }
public:
    unsigned int shader_program_handle_=0;//着色器程序句柄
    char* uniform_name_= nullptr;//uniform变量名
    bool transpose_=false;//是否转置
    float* matrix_data_=nullptr;//4x4矩阵数据
};

/// 激活并绑定纹理
class RenderTaskActiveAndBindTexture:public RenderTaskBase{
public:
    RenderTaskActiveAndBindTexture(){
        render_command_=RenderCommand::ACTIVE_AND_BIND_TEXTURE;
    }
    ~RenderTaskActiveAndBindTexture(){}
public:
    unsigned int texture_uint_;//纹理单元
    unsigned int texture_handle_;//纹理句柄
};

/// 上传1个int值
class RenderTaskSetUniform1i:public RenderTaskBase{
public:
    RenderTaskSetUniform1i(){
        render_command_=RenderCommand::SET_UNIFORM_1I;
    }
    ~RenderTaskSetUniform1i(){
        free(uniform_name_);
    }
public:
    unsigned int shader_program_handle_;//shader程序句柄
    char* uniform_name_=nullptr;//uniform变量名
    int value_;//目标值
};

/// 绑定VAO并绘制
class RenderTaskBindVAOAndDrawElements:public RenderTaskBase{
public:
    RenderTaskBindVAOAndDrawElements(){
        render_command_=RenderCommand::BIND_VAO_AND_DRAW_ELEMENTS;
    }
    ~RenderTaskBindVAOAndDrawElements(){}
public:
    unsigned int vao_handle_;
    unsigned int vertex_index_num_;//索引数量
};

/// 清除
class RenderTaskClear:public RenderTaskBase{
public:
    RenderTaskClear(){
        render_command_=RenderCommand::SET_CLEAR_FLAG_AND_CLEAR_COLOR_BUFFER;
    }
    ~RenderTaskClear(){}
public:
    unsigned int clear_flag_;
    float clear_color_r_;
    float clear_color_g_;
    float clear_color_b_;
    float clear_color_a_;
};

/// 设置模板测试函数
class RenderTaskSetStencilFunc:public RenderTaskBase{
public:
    RenderTaskSetStencilFunc(){
        render_command_=RenderCommand::SET_STENCIL_FUNC;
    }
    ~RenderTaskSetStencilFunc(){}
public:
    unsigned int stencil_func_;
    int stencil_ref_;
    unsigned int stencil_mask_;
};

/// 设置模板操作
class RenderTaskSetStencilOp:public RenderTaskBase{
public:
    RenderTaskSetStencilOp(){
        render_command_=RenderCommand::SET_STENCIL_OP;
    }
    ~RenderTaskSetStencilOp(){}
public:
    unsigned int fail_op_;
    unsigned int z_test_fail_op_;
    unsigned int z_test_pass_op_;
};

/// 设置清除模板缓冲值
class RenderTaskSetStencilBufferClearValue:public RenderTaskBase{
public:
    RenderTaskSetStencilBufferClearValue(){
        render_command_=RenderCommand::SET_STENCIL_BUFFER_CLEAR_VALUE;
    }
    ~RenderTaskSetStencilBufferClearValue(){}
public:
    int clear_value_;
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
