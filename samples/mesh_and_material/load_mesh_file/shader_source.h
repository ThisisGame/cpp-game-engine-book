//
// Created by captain on 2021/3/25.
//

#ifndef UNTITLED_SHADERSOURCE_H
#define UNTITLED_SHADERSOURCE_H
#pragma region 顶点Shader和片段Shader

static const char* vertex_shader_text =
        "#version 330 core\n"

        "uniform mat4 u_mvp;\n"

        "layout(location = 0) in  vec3 a_pos;\n"
        "layout(location = 1) in  vec4 a_color;\n"
        "layout(location = 2) in  vec2 a_uv;\n"

        "out vec4 v_color;\n"
        "out vec2 v_uv;\n"

        "void main()\n"
        "{\n"
        "    gl_Position = u_mvp * vec4(a_pos, 1.0);\n"
        "    v_color = a_color;\n"
        "    v_uv = a_uv;\n"
        "}\n";

static const char* fragment_shader_text =
        "#version 330 core\n"
        "uniform sampler2D u_diffuse_texture;"
        "in vec4 v_color;\n"
        "in vec2 v_uv;\n"
        "layout(location = 0) out vec4 o_fragColor;\n"
        "void main()\n"
        "{\n"
        "    o_fragColor = texture(u_diffuse_texture,v_uv) * v_color;\n"
        "}\n";

#pragma endregion
#endif //UNTITLED_SHADERSOURCE_H
