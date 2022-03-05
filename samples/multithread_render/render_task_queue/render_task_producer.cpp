//
// Created by captainchen on 2022/3/3.
//

#include <glm/glm.hpp>
#include "render_task_producer.h"
#include "render_task_type.h"
#include "render_task_queue.h"


/// 发出阻塞型任务：编译Shader
/// \param vertex_shader_source 顶点shader源码
/// \param fragment_shader_source 片段shader源码
/// \param result_shader_program_id 回传的Shader程序ID
void RenderTaskProducer::ProduceRenderTaskCompileShader(const char* vertex_shader_source,const char* fragment_shader_source,GLuint& result_shader_program_id){
    RenderTaskCompileShader* render_task_compile_shader=new RenderTaskCompileShader();
    render_task_compile_shader->vertex_shader_source_=vertex_shader_source;
    render_task_compile_shader->fragment_shader_source_=fragment_shader_source;
    render_task_compile_shader->need_return_result=true;//需要返回结果
    RenderTaskQueue::Push(render_task_compile_shader);
    //等待编译Shader任务结束并设置回传结果
    render_task_compile_shader->Wait();
    result_shader_program_id=render_task_compile_shader->result_shader_program_id_;
    delete render_task_compile_shader;//需要等待结果的渲染任务，需要在获取结果后删除。
}

/// 发出任务：创建VAO
/// \param shader_program_id 使用的Shader程序ID
/// \param positions 绘制的顶点位置
/// \param positions_stride 顶点位置数组的stride
/// \param colors 绘制的顶点颜色
/// \param colors_stride 顶点颜色数组的stride
/// \param result_vao 回传的VAO
void RenderTaskProducer::ProduceRenderTaskCreateVAO(GLuint shader_program_id, const void *positions, GLsizei positions_stride,
                                                    const void *colors, GLsizei colors_stride, GLuint& result_vao) {
    RenderTaskCreateVAO* render_task_create_vao=new RenderTaskCreateVAO();
    render_task_create_vao->shader_program_id_=shader_program_id;
    render_task_create_vao->positions_=positions;
    render_task_create_vao->positions_stride_=positions_stride;
    render_task_create_vao->colors_=colors;
    render_task_create_vao->colors_stride_=colors_stride;
    RenderTaskQueue::Push(render_task_create_vao);
    //等待任务结束并设置回传结果
    render_task_create_vao->Wait();
    result_vao=render_task_create_vao->result_vao_;
    delete render_task_create_vao;
}

/// 发出任务：绘制
/// \param shader_program_id 使用的Shader程序ID
/// \param vao
void RenderTaskProducer::ProduceRenderTaskDrawArray(GLuint shader_program_id, GLuint vao){
    RenderTaskDrawArray* render_task_draw_array=new RenderTaskDrawArray();
    render_task_draw_array->shader_program_id_=shader_program_id;
    render_task_draw_array->vao_=vao;
    RenderTaskQueue::Push(render_task_draw_array);//普通非阻塞型任务，交由RenderTaskConsumer使用后删除。
}

void RenderTaskProducer::ProduceRenderTaskEndFrame() {
    RenderTaskEndFrame* render_task_frame_end=new RenderTaskEndFrame();
    RenderTaskQueue::Push(render_task_frame_end);
    //等待渲染结束任务，说明渲染线程渲染完了这一帧所有的东西。
    render_task_frame_end->Wait();
    delete render_task_frame_end;//需要等待结果的任务，需要在获取结果后删除。
}