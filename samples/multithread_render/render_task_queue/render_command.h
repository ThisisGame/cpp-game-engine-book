//
// Created by captainchen on 2022/3/3.
//

#ifndef UNTITLED_RENDER_COMMAND_H
#define UNTITLED_RENDER_COMMAND_H

/// 渲染命令
enum RenderCommand {
    NONE,
    COMPILE_SHADER,//编译着色器
    DRAW_ARRAY,//绘制
    END_FRAME,//帧结束
};

#endif //UNTITLED_RENDER_COMMAND_H
