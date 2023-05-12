//
// Created by captainchen on 2023/5/12.
//

#ifndef SHADOW_MAPPING_RENDER_TARGET_STACK_H
#define SHADOW_MAPPING_RENDER_TARGET_STACK_H

#include <glad/gl.h>
#include <stack>

/// 渲染目标栈
class RenderTargetStack {
public:
    RenderTargetStack();
    ~RenderTargetStack();

    /// 压入渲染目标
    /// \param frame_buffer_object_id 帧缓冲
    void Push(GLuint frame_buffer_object_id);

    /// 弹出渲染目标
    void Pop();

    /// 检查是否为空
    /// \return 是否为空
    bool Empty();

    /// 获取栈顶渲染目标
    /// \return 栈顶渲染目标
    GLuint Top();

private:
    std::stack<GLuint> fbo_stack_;// 帧缓冲栈
};


#endif //SHADOW_MAPPING_RENDER_TARGET_STACK_H
