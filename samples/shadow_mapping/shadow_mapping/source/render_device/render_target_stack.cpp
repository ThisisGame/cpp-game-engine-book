//
// Created by captainchen on 2023/5/12.
//

#include "render_target_stack.h"

RenderTargetStack::RenderTargetStack() {

}

RenderTargetStack::~RenderTargetStack() {

}

/// 压入渲染目标
/// \param fbo_handle 帧缓冲句柄
void RenderTargetStack::Push(GLuint fbo_handle) {
    fbo_stack_.push(fbo_handle);
}

/// 弹出渲染目标
void RenderTargetStack::Pop() {
    fbo_stack_.pop();
}

/// 检查是否为空
/// \return 是否为空
bool RenderTargetStack::Empty() {
    return fbo_stack_.empty();
}

/// 获取栈顶渲染目标
/// \return 栈顶渲染目标
GLuint RenderTargetStack::Top() {
    return fbo_stack_.top();
}

