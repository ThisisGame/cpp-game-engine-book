//
// Created by captainchen on 2022/3/3.
//

#include "render_task_producer.h"
#include "easy/profiler.h"
#include "render_task_type.h"
#include "render_task_queue.h"

void RenderTaskProducer::ProduceRenderTaskUpdateScreenSize() {
    EASY_FUNCTION();
    RenderTaskUpdateScreenSize* task=new RenderTaskUpdateScreenSize();
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskSetViewportSize(int width, int height) {
    EASY_FUNCTION();
    RenderTaskSetViewportSize* task=new RenderTaskSetViewportSize();
    task->width_=width;
    task->height_=height;
    RenderTaskQueue::Push(task);
}

/// 发出任务：编译Shader
/// \param vertex_shader_source 顶点shader源码
/// \param fragment_shader_source 片段shader源码
/// \param shader_program_handle Shader程序句柄
void RenderTaskProducer::ProduceRenderTaskCompileShader(const char* vertex_shader_source,const char* fragment_shader_source,unsigned int shader_program_handle){
    EASY_FUNCTION();
    RenderTaskCompileShader* task=new RenderTaskCompileShader();
    task->vertex_shader_source_= static_cast<char *>(malloc(strlen(vertex_shader_source) + 1));
    strcpy(task->vertex_shader_source_, vertex_shader_source);

    task->fragment_shader_source_= static_cast<char *>(malloc(strlen(fragment_shader_source) + 1));
    strcpy(task->fragment_shader_source_, fragment_shader_source);

    task->shader_program_handle_=shader_program_handle;
    RenderTaskQueue::Push(task);
}

/// 发出任务：串联uniform block与binding point。
/// \param shader_program_handle Shader程序句柄
void RenderTaskProducer::ProduceRenderTaskConnectUniformBlockAndBindingPoint(unsigned int shader_program_handle){
    EASY_FUNCTION();
    RenderTaskConnectUniformBlockInstanceAndBindingPoint* task=new RenderTaskConnectUniformBlockInstanceAndBindingPoint();
    task->shader_program_handle_=shader_program_handle;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskUseShaderProgram(unsigned int shader_program_handle) {
    EASY_FUNCTION();
    RenderTaskUseShaderProgram* render_task_use_shader_program=new RenderTaskUseShaderProgram();
    render_task_use_shader_program->shader_program_handle_=shader_program_handle;
    RenderTaskQueue::Push(render_task_use_shader_program);
}

void RenderTaskProducer::ProduceRenderTaskCreateCompressedTexImage2D(unsigned int texture_handle, int width,
                                                                     int height, unsigned int texture_format,
                                                                     unsigned int compress_size,
                                                                     unsigned char *data) {
    EASY_FUNCTION();
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
    EASY_FUNCTION();
    RenderTaskCreateTexImage2D* task=new RenderTaskCreateTexImage2D();
    task->texture_handle_=texture_handle;
    task->width_=width;
    task->height_=height;
    task->gl_texture_format_=gl_texture_format;
    task->client_format_=client_format;
    task->data_type_=data_type;
    //拷贝数据
    if(data_size>0){
        task->data_= static_cast<unsigned char *>(malloc(data_size));
        memcpy(task->data_, data, data_size);
    }
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskDeleteTextures(int size, unsigned int* texture_handle_array) {
    EASY_FUNCTION();
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
    EASY_FUNCTION();
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
    EASY_FUNCTION();
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
    EASY_FUNCTION();
    RenderTaskUpdateVBOSubData* task=new RenderTaskUpdateVBOSubData();
    task->vbo_handle_=vbo_handle;
    task->vertex_data_size_=vertex_data_size;
    //拷贝数据
    task->vertex_data_= (unsigned char*)malloc(vertex_data_size);
    memcpy(task->vertex_data_, vertex_data, vertex_data_size);
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskUpdateUBOSubData(std::string uniform_block_instance_name,
                                                           std::string uniform_block_member_name, void* data){
    EASY_FUNCTION();
    RenderTaskUpdateUBOSubData* task=new RenderTaskUpdateUBOSubData();
    task->uniform_block_instance_name_=uniform_block_instance_name;
    task->uniform_block_member_name_=uniform_block_member_name;
    task->data=data;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskSetEnableState(unsigned int state, bool enable) {
    EASY_FUNCTION();
    RenderTaskSetEnableState* task=new RenderTaskSetEnableState();
    task->state_=state;
    task->enable_=enable;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskSetBlenderFunc(unsigned int source_blending_factor,
                                                         unsigned int destination_blending_factor) {
    EASY_FUNCTION();
    RenderTaskSetBlenderFunc* task=new RenderTaskSetBlenderFunc();
    task->source_blending_factor_=source_blending_factor;
    task->destination_blending_factor_=destination_blending_factor;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskSetUniformMatrix4fv(unsigned int shader_program_handle,
                                                              const char *uniform_name, bool transpose, glm::mat4& matrix) {
    EASY_FUNCTION();
    RenderTaskSetUniformMatrix4fv* task=new RenderTaskSetUniformMatrix4fv();
    task->shader_program_handle_=shader_program_handle;
    task->uniform_name_= static_cast<char *>(malloc(strlen(uniform_name) + 1));
    strcpy(task->uniform_name_, uniform_name);
    task->transpose_=transpose;
    task->matrix_= matrix;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskActiveAndBindTexture(unsigned int texture_uint, unsigned int texture_handle) {
    EASY_FUNCTION();
    RenderTaskActiveAndBindTexture* task=new RenderTaskActiveAndBindTexture();
    task->texture_uint_=texture_uint;
    task->texture_handle_=texture_handle;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskSetUniform1i(unsigned int shader_program_handle, const char *uniform_name,
                                                       int value) {
    EASY_FUNCTION();
    RenderTaskSetUniform1i* task=new RenderTaskSetUniform1i();
    task->shader_program_handle_=shader_program_handle;
    task->uniform_name_= static_cast<char *>(malloc(strlen(uniform_name) + 1));
    strcpy(task->uniform_name_, uniform_name);
    task->value_=value;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskSetUniform1f(unsigned int shader_program_handle, const char *uniform_name,
                                                       float value) {
    EASY_FUNCTION();
    RenderTaskSetUniform1f* task=new RenderTaskSetUniform1f();
    task->shader_program_handle_=shader_program_handle;
    task->uniform_name_= static_cast<char *>(malloc(strlen(uniform_name) + 1));
    strcpy(task->uniform_name_, uniform_name);
    task->value_=value;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskSetUniform3f(unsigned int shader_program_handle, const char *uniform_name,
                                                       glm::vec3 value) {
    EASY_FUNCTION();
    RenderTaskSetUniform3f* task=new RenderTaskSetUniform3f();
    task->shader_program_handle_=shader_program_handle;
    task->uniform_name_= static_cast<char *>(malloc(strlen(uniform_name) + 1));
    strcpy(task->uniform_name_, uniform_name);
    task->value_=value;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskBindVAOAndDrawElements(unsigned int vao_handle, unsigned int vertex_index_num) {
    EASY_FUNCTION();
    RenderTaskBindVAOAndDrawElements* task=new RenderTaskBindVAOAndDrawElements();
    task->vao_handle_=vao_handle;
    task->vertex_index_num_=vertex_index_num;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskSetClearFlagAndClearColorBuffer(unsigned int clear_flag, float clear_color_r, float clear_color_g, float clear_color_b, float clear_color_a){
    EASY_FUNCTION();
    RenderTaskClear* task=new RenderTaskClear();
    task->clear_flag_=clear_flag;
    task->clear_color_r_=clear_color_r;
    task->clear_color_g_=clear_color_g;
    task->clear_color_b_=clear_color_b;
    task->clear_color_a_=clear_color_a;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskSetStencilFunc(unsigned int stencil_func,int stencil_ref,unsigned int stencil_mask){
    EASY_FUNCTION();
    RenderTaskSetStencilFunc* task=new RenderTaskSetStencilFunc();
    task->stencil_func_=stencil_func;
    task->stencil_ref_=stencil_ref;
    task->stencil_mask_=stencil_mask;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskSetStencilOp(unsigned int fail_op_,unsigned int z_test_fail_op_,unsigned int z_test_pass_op_){
    EASY_FUNCTION();
    RenderTaskSetStencilOp* task=new RenderTaskSetStencilOp();
    task->fail_op_=fail_op_;
    task->z_test_fail_op_=z_test_fail_op_;
    task->z_test_pass_op_=z_test_pass_op_;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskSetStencilBufferClearValue(int clear_value){
    EASY_FUNCTION();
    RenderTaskSetStencilBufferClearValue* task=new RenderTaskSetStencilBufferClearValue();
    task->clear_value_=clear_value;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskCreateFBO(int fbo_handle,unsigned short width,unsigned short height,unsigned int color_texture_handle,unsigned int depth_texture_handle){
    RenderTaskCreateFBO* task=new RenderTaskCreateFBO();
    task->fbo_handle_=fbo_handle;
    task->width_=width;
    task->height_=height;
    task->color_texture_handle_=color_texture_handle;
    task->depth_texture_handle_=depth_texture_handle;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskBindFBO(int fbo_handle){
    RenderTaskBindFBO* task=new RenderTaskBindFBO();
    task->fbo_handle_=fbo_handle;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskUnBindFBO(int fbo_handle){
    RenderTaskUnBindFBO* task=new RenderTaskUnBindFBO();
    task->fbo_handle_=fbo_handle;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskDeleteFBO(int fbo_handle){
    RenderTaskDeleteFBO* task=new RenderTaskDeleteFBO();
    task->fbo_handle_=fbo_handle;
    RenderTaskQueue::Push(task);
}

void RenderTaskProducer::ProduceRenderTaskEndFrame() {
    EASY_FUNCTION();
    RenderTaskEndFrame* render_task_frame_end=new RenderTaskEndFrame();
    RenderTaskQueue::Push(render_task_frame_end);
    //等待渲染结束任务，说明渲染线程渲染完了这一帧所有的东西。
    render_task_frame_end->Wait();
    delete render_task_frame_end;//需要等待结果的任务，需要在获取结果后删除。
}