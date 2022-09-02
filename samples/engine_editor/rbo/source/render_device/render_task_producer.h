//
// Created by captainchen on 2022/3/3.
//

#ifndef UNTITLED_RENDER_TASK_PRODUCER_H
#define UNTITLED_RENDER_TASK_PRODUCER_H

#include <string>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include "types/type_define.h"


/// 渲染任务生产者
class RenderTaskProducer {
public:
    /// 发出任务：更新游戏窗口尺寸
    static void ProduceRenderTaskUpdateScreenSize(ushort view_port_width,ushort view_port_height);

    /// 发出任务：编译Shader
    /// \param vertex_shader_source 顶点shader源码
    /// \param fragment_shader_source 片段shader源码
    /// \param shader_program_handle Shader程序句柄
    static void ProduceRenderTaskCompileShader(const char* vertex_shader_source,const char* fragment_shader_source,uint shader_program_handle);

    /// 发出任务：串联uniform block与binding point。
    /// \param shader_program_handle Shader程序句柄
    static void ProduceRenderTaskConnectUniformBlockAndBindingPoint(uint shader_program_handle);

    /// 发出任务：使用Shader程序
    static void ProduceRenderTaskUseShaderProgram(uint shader_program_handle);

    /// 发出任务：创建压缩纹理
    /// \param texture_handle 纹理句柄
    /// \param width
    /// \param height
    /// \param texture_format 压缩纹理格式
    /// \param compress_size
    /// \param data 压缩纹理数据，注意函数里是拷贝内存块。
    static void ProduceRenderTaskCreateCompressedTexImage2D(uint texture_handle, int width, int height, uint texture_format, uint compress_size,
                                                            unsigned char *data);

    /// 发出任务：创建纹理
    /// \param texture_handle
    /// \param width
    /// \param height
    /// \param gl_texture_format
    /// \param client_format
    /// \param data_type
    /// \param data_size
    /// \param data
    static void ProduceRenderTaskCreateTexImage2D(uint texture_handle, int width, int height, uint gl_texture_format, uint client_format,uint data_type, uint data_size, unsigned char *data);

    /// 发出任务：删除一个或多个Texture
    /// \param size
    /// \param texture_handle_array 指定要删除的Texture的句柄数组，注意函数里是拷贝内存块。
    static void ProduceRenderTaskDeleteTextures(int size,uint* texture_handle_array);

    /// 发出任务：局部更新Texture
    /// \param texture_handle 纹理句柄
    /// \param x
    /// \param y
    /// \param width
    /// \param height
    /// \param client_format
    /// \param data_type
    /// \param data 指定要更新的数据，注意函数里是拷贝内存块。
    /// \param data_size
    static void ProduceRenderTaskUpdateTextureSubImage2D(uint texture_handle, int x, int y, int width, int height, uint client_format, uint data_type,
                                                         unsigned char *data,uint data_size);

    /// 发出任务：创建VAO
    /// \param shader_program_handle
    /// \param vao_handle
    /// \param vertex_data_size
    /// \param vertex_data_stride
    /// \param vertex_data
    /// \param vertex_index_data_size
    /// \param vertex_index_data
    static void ProduceRenderTaskCreateVAO(uint shader_program_handle,uint vao_handle,uint vbo_handle,uint vertex_data_size,uint vertex_data_stride,void* vertex_data,uint vertex_index_data_size,void* vertex_index_data);

    /// 发出任务：更新VBO
    /// \param vbo_handle
    /// \param vertex_data_size
    /// \param vertex_data
    static void ProduceRenderTaskUpdateVBOSubData(uint vbo_handle,uint vertex_data_size,void* vertex_data);

    /// 发出任务：创建UBO
    /// \param shader_program_handle
    /// \param ubo_handle
    /// \param uniform_block_instance_name
    /// \param uniform_block_data_size
    /// \param uniform_block_data
    static void ProduceRenderTaskUpdateUBOSubData(std::string uniform_block_instance_name, std::string uniform_block_member_name, void* data);


    /// 发出任务：设置状态,开启或关闭
    static void ProduceRenderTaskSetEnableState(uint state,bool enable);

    /// 发出任务：设置混合函数
    /// \param source_blending_factor 源混合因子
    /// \param destination_blending_factor 目标混合因子
    static void ProduceRenderTaskSetBlenderFunc(uint source_blending_factor,uint destination_blending_factor);

    /// 发出任务：设置4x4矩阵
    /// \param shader_program_handle
    /// \param uniform_name
    /// \param transpose
    /// \param value
    /// \param value_data_size
    static void ProduceRenderTaskSetUniformMatrix4fv(uint shader_program_handle, const char* uniform_name, bool transpose, glm::mat4& matrix);

    /// 激活并绑定纹理
    /// \param texture_uint
    /// \param texture_handle
    static void ProduceRenderTaskActiveAndBindTexture(uint texture_uint,uint texture_handle);

    /// 上传1个int值
    /// \param shader_program_handle
    /// \param uniform_name
    /// \param value
    static void ProduceRenderTaskSetUniform1i(uint shader_program_handle, const char* uniform_name, int value);

    /// 上传1个float值
    /// \param shader_program_handle
    /// \param uniform_name
    /// \param value
    static void ProduceRenderTaskSetUniform1f(uint shader_program_handle, const char* uniform_name, float value);

    /// 上传1个 vec3
    /// \param shader_program_handle
    /// \param uniform_name
    /// \param value
    static void ProduceRenderTaskSetUniform3f(uint shader_program_handle, const char* uniform_name, glm::vec3 value);

    /// 绑定VAO并绘制
    /// \param vao_handle
    /// \param inex_count
    static void ProduceRenderTaskBindVAOAndDrawElements(uint vao_handle,uint vertex_index_num);

    /// 设置clear_flag并且清除颜色缓冲
    /// \param clear_flag
    /// \param clear_color_r
    /// \param clear_color_g
    /// \param clear_color_b
    /// \param clear_color_a
    static void ProduceRenderTaskSetClearFlagAndClearColorBuffer(uint clear_flag, float clear_color_r, float clear_color_g, float clear_color_b, float clear_color_a);

    /// 设置模板测试函数
    /// \param stencil_func
    /// \param stencil_ref
    /// \param stencil_mask
    static void ProduceRenderTaskSetStencilFunc(uint stencil_func,int stencil_ref,uint stencil_mask);

    /// 设置模板操作
    /// \param fail_op_
    /// \param z_test_fail_op_
    /// \param z_test_pass_op_
    static void ProduceRenderTaskSetStencilOp(uint fail_op_,uint z_test_fail_op_,uint z_test_pass_op_);

    /// 设置清除模板缓冲值
    static void ProduceRenderTaskSetStencilBufferClearValue(int clear_value);

    /// 创建帧缓冲区对象(FBO)
    /// \param fbo_handle FBO句柄
    /// \param width 帧缓冲区尺寸(宽)
    /// \param height 帧缓冲区尺寸(高)
    static void ProduceRenderTaskCreateFBO(int fbo_handle,ushort width,ushort height);

    /// 绑定使用帧缓冲区对象(FBO)
    static void ProduceRenderTaskBindFBO(int fbo_handle);

    /// 取消使用帧缓冲区对象(FBO)
    static void ProduceRenderTaskUnBindFBO(int fbo_handle);

    /// 删除帧缓冲区对象(FBO)
    static void ProduceRenderTaskDeleteFBO(int fbo_handle);

    /// 创建渲染缓冲区对象(RBO)
    /// \param rbo_handle RBO句柄
    /// \param width 缓冲区尺寸(宽)
    /// \param height 缓冲区尺寸(高)
    static void ProduceRenderTaskCreateRBO(int rbo_handle,ushort width,ushort height);

    /// 删除渲染缓冲区对象(RBO)
    static void ProduceRenderTaskDeleteRBO(int rbo_handle);

    /// FBO附着点指定RBO任务
    static void ProduceRenderTaskFBOAttachRBO(int fbo_handle,int rbo_handle);

    /// FBO附着点指定Texture任务
    static void ProduceRenderTaskFBOAttachTexture(int fbo_handle,int rbo_handle,uint color_texture_handle,uint depth_texture_handle);

    /// 将像素块从读取的帧缓冲区(GL_READ_FRAMEBUFFER)复制到绘制帧缓冲区(GL_DRAW_FRAMEBUFFER) 任务
    /// \param src_fbo_handle 源FrameBuffer(GL_READ_FRAMEBUFFER) 句柄
    /// \param dst_fbo_handle
    /// \param src_x
    /// \param src_y
    /// \param src_width
    /// \param src_height
    /// \param dst_x
    /// \param dst_y
    /// \param dst_width
    /// \param dst_height
    /// \param mask 标志的按位或，指示要复制哪些缓冲区，允许的标志是GL_COLOR_BUFFER_BIT，GL_DEPTH_BUFFER_BIT和GL_STENCIL_BUFFER_BIT。
    /// \param filter 指定在拉伸图像时要应用的插值。 必须为GL_NEAREST或GL_LINEAR。
    static void ProduceRenderTaskBlitFrameBuffer(uint src_fbo_handle,uint dst_fbo_handle,int src_x,int src_y,int src_width,int src_height,int dst_x,int dst_y,int dst_width,int dst_height,uint mask,uint filter);

    /// 发出特殊任务：渲染结束
    static void ProduceRenderTaskEndFrame();
};


#endif //UNTITLED_RENDER_TASK_PRODUCER_H
