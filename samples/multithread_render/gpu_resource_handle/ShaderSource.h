//
// Created by captain on 2021/3/25.
//

#ifndef UNTITLED_SHADERSOURCE_H
#define UNTITLED_SHADERSOURCE_H

//顶点着色器代码
static const char* vertex_shader_text =
        "#version 330\n"

        "uniform mat4 u_mvp;\n"

        "in  vec3 a_pos;\n"
        "in  vec4 a_color;\n"

        "out vec4 v_color;\n"

        "void main()\n"
        "{\n"
        "    gl_Position = vec4(a_pos, 1.0);\n"
        "    v_color = a_color;\n"
        "}\n";

//片段着色器代码
static const char* fragment_shader_text =
        "#version 330\n"
        "in vec4 v_color;\n"
        "out vec4 o_fragColor;\n"
        "void main()\n"
        "{\n"
        "    o_fragColor = v_color;\n"
        "}\n";

#endif //UNTITLED_SHADERSOURCE_H
