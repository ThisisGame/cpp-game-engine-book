//
// Created by captainchen on 2022/3/3.
//

#include "render_task_producer.h"
#include "render_task_type.h"
#include "render_task_queue.h"

void RenderTaskProducer::ProduceRenderTaskUpdateScreenSize() {
    RenderTaskUpdateScreenSize* task=new RenderTaskUpdateScreenSize();
    RenderTaskQueue::Push(task);
}

/// 发出任务：编译Shader
/// \param vertex_shader_source 顶点shader源码
/// \param fragment_shader_source 片段shader源码
/// \param shader_program_handle Shader程序句柄
void RenderTaskProducer::ProduceRenderTaskCompileShader(const char* vertex_shader_source,const char* fragment_shader_source,unsigned int shader_program_handle){
    RenderTaskCompileShader* render_task_compile_shader=new RenderTaskCompileShader();
    render_task_compile_shader->vertex_shader_source_=vertex_shader_source;
    render_task_compile_shader->fragment_shader_source_=fragment_shader_source;
    render_task_compile_shader->shader_program_handle_=shader_program_handle;
    RenderTaskQueue::Push(render_task_compile_shader);
}

void RenderTaskProducer::ProduceRenderTaskUseShaderProgram(unsigned int shader_program_handle) {
    RenderTaskUseShaderProgram* render_task_use_shader_program=new RenderTaskUseShaderProgram();
    render_task_use_shader_program->shader_program_handle_=shader_program_handle;
    RenderTaskQueue::Push(render_task_use_shader_program);
}

void RenderTaskProducer::ProduceRenderTaskCreateCompressedTexImage2D(unsigned int texture_handle, int width,
                                                                     int height, unsigned int texture_format,
                                                                     unsigned int compress_size,
                                                                     unsigned char *data) {
    RenderTaskCreateCompressedTexImage2D* task=new RenderTaskCreateCompressedTexImage2D();
    task->texture_handle_=texture_handle;
    task->width_=width;
    task->height_=height;
    task->texture_format_=texture_format;
    task->compress_size_=compress_size;
    //拷贝数据
    task->data_= static_cast<unsigned char *>(malloc(compress_size));
    memcpy(task->data_, data, compress_size);
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskCreateTexImage2D(unsigned int texture_handle, int width, int height,
                                                           unsigned int gl_texture_format, unsigned int client_format,
                                                           unsigned int data_type, unsigned int data_size,
                                                           unsigned char *data) {
    RenderTaskCreateTexImage2D* task=new RenderTaskCreateTexImage2D();
    task->texture_handle_=texture_handle;
    task->width_=width;
    task->height_=height;
    task->gl_texture_format_=gl_texture_format;
    task->client_format_=client_format;
    task->data_type_=data_type;
    //拷贝数据
    task->data_= static_cast<unsigned char *>(malloc(data_size));
    memcpy(task->data_, data, data_size);
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskDeleteTextures(int size, unsigned int* texture_handle_array) {
    RenderTaskDeleteTextures* task=new RenderTaskDeleteTextures();
    //拷贝数据
    task->texture_handle_array_= (unsigned int*)malloc(sizeof(unsigned int)*size);
    memcpy(task->texture_handle_array_, texture_handle_array, sizeof(unsigned int) * size);
    task->texture_count_=size;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskUpdateTextureSubImage2D(unsigned int texture_handle, int x, int y, int width, int height,
                                                                  unsigned int client_format, unsigned int data_type,
                                                                  unsigned char *data,unsigned int data_size) {
    RenderTaskUpdateTextureSubImage2D* task=new RenderTaskUpdateTextureSubImage2D();
    task->texture_handle_=texture_handle;
    task->x_=x;
    task->y_=y;
    task->width_=width;
    task->height_=height;
    task->client_format_=client_format;
    task->data_type_=data_type;
    //拷贝数据
    task->data_= (unsigned char*)malloc(data_size);
    memcpy(task->data_, data, data_size);
    task->data_size_=data_size;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskCreateVAO(unsigned int shader_program_handle, unsigned int vao_handle,unsigned int vbo_handle,
                                                    unsigned int vertex_data_size, unsigned int vertex_data_stride,
                                                    void *vertex_data, unsigned int vertex_index_data_size,
                                                    void *vertex_index_data) {
    RenderTaskCreateVAO* task=new RenderTaskCreateVAO();
    task->shader_program_handle_=shader_program_handle;
    task->vao_handle_=vao_handle;
    task->vbo_handle_=vbo_handle;
    task->vertex_data_size_=vertex_data_size;
    task->vertex_data_stride_=vertex_data_stride;
    //拷贝数据
    task->vertex_data_= (unsigned char*)malloc(vertex_data_size);
    memcpy(task->vertex_data_, vertex_data, vertex_data_size);
    task->vertex_index_data_size_=vertex_index_data_size;
    //拷贝数据
    task->vertex_index_data_= (unsigned char*)malloc(vertex_index_data_size);
    memcpy(task->vertex_index_data_, vertex_index_data, vertex_index_data_size);
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskUpdateVBOSubData(unsigned int vbo_handle, unsigned int vertex_data_size,
                                                           void *vertex_data) {
    RenderTaskUpdateVBOSubData* task=new RenderTaskUpdateVBOSubData();
    task->vbo_handle_=vbo_handle;
    task->vertex_data_size_=vertex_data_size;
    //拷贝数据
    task->vertex_data_= (unsigned char*)malloc(vertex_data_size);
    memcpy(task->vertex_data_, vertex_data, vertex_data_size);
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskSetEnableState(unsigned int state, bool enable) {
    RenderTaskSetEnableState* task=new RenderTaskSetEnableState();
    task->state_=state;
    task->enable_=enable;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskSetBlenderFunc(unsigned int source_blending_factor,
                                                         unsigned int destination_blending_factor) {
    RenderTaskSetBlenderFunc* task=new RenderTaskSetBlenderFunc();
    task->source_blending_factor_=source_blending_factor;
    task->destination_blending_factor_=destination_blending_factor;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskEndFrame() {
    RenderTaskEndFrame* render_task_frame_end=new RenderTaskEndFrame();
    RenderTaskQueue::Push(render_task_frame_end);
    //等待渲染结束任务，说明渲染线程渲染完了这一帧所有的东西。
    render_task_frame_end->Wait();
    delete render_task_frame_end;//需要等待结果的任务，需要在获取结果后删除。
}