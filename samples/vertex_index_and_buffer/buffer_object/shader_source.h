//
// Created by captain on 2021/3/25.
//

#ifndef UNTITLED_SHADERSOURCE_H
#define UNTITLED_SHADERSOURCE_H
#pragma region 顶点Shader和片段Shader

static const char* vertex_shader_text =
        "#version 110\n"

        "uniform mat4 u_mvp;\n"

        "attribute  vec3 a_pos;\n"
        "attribute  vec4 a_color;\n"
        "attribute  vec2 a_uv;\n"

        "varying vec4 v_color;\n"
        "varying vec2 v_uv;\n"

        "void main()\n"
        "{\n"
        "    gl_Position = u_mvp * vec4(a_pos, 1.0);\n"
        "    v_color = a_color;\n"
        "    v_uv = a_uv;\n"
        "}\n";

static const char* fragment_shader_text =
        "#version 110\n"
        "uniform sampler2D u_diffuse_texture;"
        "varying vec4 v_color;\n"
        "varying vec2 v_uv;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = texture2D(u_diffuse_texture,v_uv);\n"
        "}\n";

#pragma endregion
#endif //UNTITLED_SHADERSOURCE_H
