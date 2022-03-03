//
// Created by captainchen on 2022/3/3.
//

#include "render_task_producer.h"
#include "render_task_type.h"
#include "render_task_consumer.h"

void RenderTaskProducer::ProduceRenderTaskUpdateScreenSize(int &width, int &height) {
    RenderTaskUpdateScreenSize* task=new RenderTaskUpdateScreenSize();
    RenderTaskConsumer::PushRenderTask(task);
    task->Wait();
    width=task->width_;
    height=task->height_;
    delete task;
}

void RenderTaskProducer::ProduceRenderTaskDeleteTextures(int size, GLuint *gl_texture_ids) {
    RenderTaskDeleteTextures* task=new RenderTaskDeleteTextures();
    task->texture_ids_= (GLuint*)malloc(sizeof(GLuint)*size);
    memcpy(task->texture_ids_,gl_texture_ids,sizeof(GLuint)*size);
    task->texture_count_=size;
    RenderTaskConsumer::PushRenderTask(task);
}

void RenderTaskProducer::ProduceRenderTaskUpdateTextureSubImage2D(GLuint gl_texture_id,int x, int y, int width, int height,
                                                                  unsigned int client_format, unsigned int data_type,
                                                                  unsigned char *data) {
    RenderTaskUpdateTextureSubImage2D* task=new RenderTaskUpdateTextureSubImage2D();
    task->gl_texture_id_=gl_texture_id;
    task->x_=x;
    task->y_=y;
    task->width_=width;
    task->height_=height;
    task->client_format_=client_format;
    task->data_type_=data_type;
    task->data_=data;
    RenderTaskConsumer::PushRenderTask(task);
}

void RenderTaskProducer::ProduceRenderTaskEndFrame() {
    RenderTaskEndFrame* render_task_frame_end=new RenderTaskEndFrame();
    RenderTaskConsumer::PushRenderTask(render_task_frame_end);
    //等待渲染结束任务，说明渲染线程渲染完了这一帧所有的东西。
    render_task_frame_end->Wait();
    delete render_task_frame_end;//需要等待结果的任务，需要在获取结果后删除。
}