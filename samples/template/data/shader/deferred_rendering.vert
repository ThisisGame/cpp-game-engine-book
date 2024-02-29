#version 330 core

layout(location = 0) in  vec3 a_pos;
layout(location = 1) in  vec4 a_color;
layout(location = 2) in  vec2 a_uv;
layout(location = 3) in  vec3 a_normal;

out vec2 v_uv;

void main()
{
    gl_Position = vec4(a_pos, 1.0);

    v_uv = a_uv;
}