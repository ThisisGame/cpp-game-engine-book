//
// Created by captainchen on 2022/3/3.
//

#ifndef UNTITLED_RENDER_TASK_PRODUCER_H
#define UNTITLED_RENDER_TASK_PRODUCER_H

#include <glad/gl.h>

/// 渲染任务生产者
class RenderTaskProducer {
public:
    /// 发出任务：更新游戏窗口尺寸，并将渲染线程返回的最新的尺寸设置到传入的参数
    /// \param width 被设置的宽
    /// \param height 被设置的高
    static void ProduceRenderTaskUpdateScreenSize(int& width, int& height);

    /// 发出任务：删除一个或多个Texture
    /// \param size
    /// \param gl_texture_ids
    static void ProduceRenderTaskDeleteTextures(int size,GLuint* gl_texture_ids);

    /// 发出任务：局部更新Texture
    /// \param gl_texture_id
    /// \param x
    /// \param y
    /// \param width
    /// \param height
    /// \param client_format
    /// \param data_type
    /// \param data
    static void ProduceRenderTaskUpdateTextureSubImage2D(GLuint gl_texture_id,int x, int y, int width, int height, unsigned int client_format, unsigned int data_type,
                                                         unsigned char *data);

    /// 发出特殊任务：渲染结束
    static void ProduceRenderTaskEndFrame();
};


#endif //UNTITLED_RENDER_TASK_PRODUCER_H
