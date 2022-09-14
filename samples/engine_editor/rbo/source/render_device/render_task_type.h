//
// Created by captainchen on 2022/3/3.
//

#ifndef UNTITLED_RENDER_TASK_TYPE_H
#define UNTITLED_RENDER_TASK_TYPE_H

#include <stdlib.h>
#include <glm/glm.hpp>
#include "render_command.h"

/// 渲染任务基类
class RenderTaskBase{
public:
    RenderTaskBase(){}
    virtual ~RenderTaskBase(){}
public:
    RenderCommand render_command_;//渲染命令
    bool need_return_result_ = false;//是否需要回传结果
    bool return_result_set_ = false;//是否设置好了回传结果
};

/// 需要回传结果的阻塞性任务
class RenderTaskNeedReturnResult: public RenderTaskBase{
public:
    RenderTaskNeedReturnResult(){
        render_command_=RenderCommand::NONE;
        need_return_result_=true;
    }
    ~RenderTaskNeedReturnResult(){}
    /// 等待任务在渲染线程执行完毕，并设置回传结果。主线程拿到结果后才能执行下一步代码。
    virtual void Wait(){
        while(return_result_set_ == false){}
    }
};

/// 更新游戏画面尺寸任务
class RenderTaskUpdateScreenSize:public RenderTaskBase{
public:
    RenderTaskUpdateScreenSize(){
        render_command_=RenderCommand::UPDATE_SCREEN_SIZE;
    }
    ~RenderTaskUpdateScreenSize(){}
public:
    unsigned short view_port_width_;
    unsigned short view_port_height_;
};

/// 限定在窗口的渲染区域任务
class RenderTaskSetRenderRectInWindow:public RenderTaskBase{
public:
    RenderTaskSetRenderRectInWindow(){
        render_command_=RenderCommand::SET_RENDER_RECT_IN_WINDOW;
    }
    ~RenderTaskSetRenderRectInWindow(){}
public:
    unsigned short x_;
    unsigned short y_;
    unsigned short width_;
    unsigned short height_;
};

/// 编译着色器任务
class RenderTaskCompileShader: public RenderTaskBase{
public:
    RenderTaskCompileShader(){
        render_command_=RenderCommand::COMPILE_SHADER;
    }
    ~RenderTaskCompileShader(){
        free(vertex_shader_source_);
        free(fragment_shader_source_);
    }
public:
    char* vertex_shader_source_= nullptr;
    char* fragment_shader_source_= nullptr;
    unsigned int shader_program_handle_= 0;
};

/// 任务：串联uniform block实例与binding point。
class RenderTaskConnectUniformBlockInstanceAndBindingPoint: public RenderTaskBase{
public:
    RenderTaskConnectUniformBlockInstanceAndBindingPoint(){
        render_command_=RenderCommand::CONNECT_UNIFORM_BLOCK_INSTANCE_AND_BINDING_POINT;
    }
    ~RenderTaskConnectUniformBlockInstanceAndBindingPoint(){
    }
public:
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
        data_= nullptr;
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

/// 创建UBO任务
class RenderTaskCreateUBO: public RenderTaskBase{
public:
    RenderTaskCreateUBO(){
        render_command_=RenderCommand::CREATE_UBO;
    }
    ~RenderTaskCreateUBO(){
    }
public:
    unsigned int shader_program_handle_=0;//着色器程序句柄
    unsigned int ubo_handle_=0;//UBO句柄
    char* uniform_block_name_;
    unsigned short uniform_block_data_size_;//数据大小
    void* uniform_block_data_;//数据
};

/// 更新UBO数据
class RenderTaskUpdateUBOSubData:public RenderTaskBase{
public:
    RenderTaskUpdateUBOSubData(){
        render_command_=RenderCommand::UPDATE_UBO_SUB_DATA;
    }
    ~RenderTaskUpdateUBOSubData(){
        free(data);
    }
public:
    std::string uniform_block_instance_name_;
    std::string uniform_block_member_name_;
    void* data;
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
    }
public:
    unsigned int shader_program_handle_=0;//着色器程序句柄
    char* uniform_name_= nullptr;//uniform变量名
    bool transpose_=false;//是否转置
    glm::mat4 matrix_;//4x4矩阵数据
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

/// 上传1个uniform值
class RenderTaskSetUniform:public RenderTaskBase{
public:
    RenderTaskSetUniform(){}
    ~RenderTaskSetUniform(){
        free(uniform_name_);
    }
public:
    unsigned int shader_program_handle_;//shader程序句柄
    char* uniform_name_=nullptr;//uniform变量名
};

/// 上传1个int值
class RenderTaskSetUniform1i:public RenderTaskSetUniform{
public:
    RenderTaskSetUniform1i(){
        render_command_=RenderCommand::SET_UNIFORM_1I;
    }
public:
    int value_;//目标值
};

/// 上传1个float值
class RenderTaskSetUniform1f:public RenderTaskSetUniform{
public:
    RenderTaskSetUniform1f(){
        render_command_=RenderCommand::SET_UNIFORM_1F;
    }
public:
    float value_;//目标值
};

/// 上传1个 vec3
class RenderTaskSetUniform3f:public RenderTaskSetUniform{
public:
    RenderTaskSetUniform3f(){
        render_command_=RenderCommand::SET_UNIFORM_3F;
    }
public:
    glm::vec3 value_;//目标值
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


/// 创建FBO任务
class RenderTaskCreateFBO: public RenderTaskBase{
public:
    RenderTaskCreateFBO(){
        render_command_=RenderCommand::CREATE_FBO;
    }
    ~RenderTaskCreateFBO(){
    }
public:
    unsigned int fbo_handle_=0;//FBO句柄
    unsigned short width_=128;//帧缓冲区尺寸(宽)
    unsigned short height_=128;//帧缓冲区尺寸(高)
};

/// 绑定使用FBO任务
class RenderTaskBindFBO: public RenderTaskBase{
public:
    RenderTaskBindFBO(){
        render_command_=RenderCommand::BIND_FBO;
    }
    ~RenderTaskBindFBO(){
    }
public:
    unsigned int fbo_handle_=0;//FBO句柄
};

/// 取消使用FBO任务
class RenderTaskUnBindFBO: public RenderTaskBase{
public:
    RenderTaskUnBindFBO(){
        render_command_=RenderCommand::UNBIND_FBO;
    }
    ~RenderTaskUnBindFBO(){
    }
public:
    unsigned int fbo_handle_=0;//FBO句柄
};

/// 删除帧缓冲区对象(FBO)任务
class RenderTaskDeleteFBO: public RenderTaskBase{
public:
    RenderTaskDeleteFBO(){
        render_command_=RenderCommand::DELETE_FBO;
    }
    ~RenderTaskDeleteFBO(){
    }
public:
    unsigned int fbo_handle_=0;//FBO句柄
};

/// 创建RBO任务
class RenderTaskCreateRBO: public RenderTaskBase{
public:
    RenderTaskCreateRBO(){
        render_command_=RenderCommand::CREATE_RBO;
    }
    ~RenderTaskCreateRBO(){
    }
public:
    unsigned int rbo_handle_=0;//RBO句柄
    unsigned short width_=128;
    unsigned short height_=128;
};

/// 删除RBO任务
class RenderTaskDeleteRBO: public RenderTaskBase{
public:
    RenderTaskDeleteRBO(){
        render_command_=RenderCommand::DELETE_RBO;
    }
    ~RenderTaskDeleteRBO(){
    }
public:
    unsigned int rbo_handle_=0;//RBO句柄
};

/// FBO附着点指定RBO任务
class RenderTaskFBOAttachRBO: public RenderTaskBase{
public:
    RenderTaskFBOAttachRBO(){
        render_command_=RenderCommand::FBO_ATTACH_RBO;
    }
    ~RenderTaskFBOAttachRBO(){
    }
public:
    unsigned int fbo_handle_=0;//FBO句柄
    unsigned int rbo_handle_=0;//RBO句柄
};

/// FBO附着点指定Texture 任务
class RenderTaskFBOAttachTexture: public RenderTaskBase{
public:
    RenderTaskFBOAttachTexture(){
        render_command_=RenderCommand::FBO_ATTACH_TEXTURE;
    }
    ~RenderTaskFBOAttachTexture(){
    }
public:
    unsigned int fbo_handle_=0;//FBO句柄
    unsigned int color_texture_handle_=0;//FBO颜色附着点关联的颜色纹理
    unsigned int depth_texture_handle_=0;//FBO深度附着点关联的深度纹理
};

/// 将像素块从读取的帧缓冲区(GL_READ_FRAMEBUFFER)复制到绘制帧缓冲区(GL_DRAW_FRAMEBUFFER) 任务
class RenderTaskBlitFrameBuffer: public RenderTaskBase{
public:
    RenderTaskBlitFrameBuffer(){
        render_command_=RenderCommand::BLIT_FRAME_BUFFER;
    }
    ~RenderTaskBlitFrameBuffer(){
    }
public:
    unsigned int src_fbo_handle_=0;//源FrameBuffer(GL_READ_FRAMEBUFFER) 句柄
    unsigned int dst_fbo_handle_=0;//目标FrameBuffer(GL_DRAW_FRAMEBUFFER) 句柄
    int src_x_=0;
    int src_y_=0;
    int src_width_=0;
    int src_height_=0;
    int dst_x_=0;
    int dst_y_=0;
    int dst_width_=0;
    int dst_height_=0;
    unsigned int mask_=0;//标志的按位或，指示要复制哪些缓冲区，允许的标志是GL_COLOR_BUFFER_BIT，GL_DEPTH_BUFFER_BIT和GL_STENCIL_BUFFER_BIT。
    unsigned int filter_=0;//指定在拉伸图像时要应用的插值。 必须为GL_NEAREST或GL_LINEAR。
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
