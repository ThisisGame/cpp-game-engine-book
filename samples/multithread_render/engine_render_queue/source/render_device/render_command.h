//
// Created by captainchen on 2022/3/3.
//

#ifndef UNTITLED_RENDER_COMMAND_H
#define UNTITLED_RENDER_COMMAND_H

/// 渲染命令
enum RenderCommand {
    NONE,
    UPDATE_SCREEN_SIZE,//更新游戏画面尺寸
    COMPILE_SHADER,//编译着色器
    DELETE_TEXTURES,//删除纹理
    UPDATE_TEXTURE_SUB_IMAGE2D,//局部更新Texture
    DRAW_ARRAY,//绘制
    END_FRAME,//帧结束
};

#endif //UNTITLED_RENDER_COMMAND_H
