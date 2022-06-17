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
    USE_SHADER_PROGRAM,//使用着色器程序
    CREATE_VAO,//创建VAO
    CREATE_UBO,//创建UBO
    UPDATE_VBO_SUB_DATA,//更新VBO数据
    CREATE_COMPRESSED_TEX_IMAGE2D,//创建压缩纹理
    CREATE_TEX_IMAGE2D,//创建纹理
    DELETE_TEXTURES,//删除纹理
    UPDATE_TEXTURE_SUB_IMAGE2D,//局部更新Texture
    SET_ENABLE_STATE,//设置状态,开启或关闭
    SET_BLENDER_FUNC,//设置混合函数
    SET_UNIFORM_MATRIX_4FV,//上传4x4矩阵
    ACTIVE_AND_BIND_TEXTURE,//激活并绑定纹理
    SET_UNIFORM_1I,//上传1个int值
    SET_UNIFORM_1F,//上传1个float值
    SET_UNIFORM_3F,//上传1个vec3
    BIND_VAO_AND_DRAW_ELEMENTS,//绑定VAO并绘制
    SET_CLEAR_FLAG_AND_CLEAR_COLOR_BUFFER,//设置clear_flag并且清除颜色缓冲
    SET_STENCIL_FUNC,//设置模板测试函数
    SET_STENCIL_OP,//设置模板操作
    SET_STENCIL_BUFFER_CLEAR_VALUE,//设置清除模板缓冲值
    END_FRAME,//帧结束
};

#endif //UNTITLED_RENDER_COMMAND_H
