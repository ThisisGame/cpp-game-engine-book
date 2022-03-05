//
// Created by captainchen on 2022/3/3.
//

#ifndef UNTITLED_RENDER_TASK_PRODUCER_H
#define UNTITLED_RENDER_TASK_PRODUCER_H

#include <glad/gl.h>

/// 渲染任务生产者
class RenderTaskProducer {
public:
    /// 发出任务：编译Shader
    /// \param vertex_shader_source 顶点shader源码
    /// \param fragment_shader_source 片段shader源码
    /// \param shader_program_handle Shader程序句柄
    static void ProduceRenderTaskCompileShader(const char* vertex_shader_source,const char* fragment_shader_source,unsigned int shader_program_handle);

    /// 发出任务：创建VAO
    /// \param shader_program_handle 使用的Shader程序句柄
    /// \param positions 绘制的顶点位置
    /// \param positions_stride 顶点位置数组的stride
    /// \param colors 绘制的顶点颜色
    /// \param colors_stride 顶点颜色数组的stride
    /// \param vao_handle VAO句柄
    static void ProduceRenderTaskCreateVAO(GLuint shader_program_handle, const void* positions, GLsizei positions_stride, const void* colors, GLsizei colors_stride, unsigned int vao_handle);

    /// 发出任务：绘制
    /// \param shader_program_handle 使用的Shader程序句柄
    /// \param vao_handle VAO句柄
    static void ProduceRenderTaskDrawArray(GLuint shader_program_handle, GLuint vao_handle);

    /// 发出特殊任务：渲染结束
    static void ProduceRenderTaskEndFrame();
};


#endif //UNTITLED_RENDER_TASK_PRODUCER_H
