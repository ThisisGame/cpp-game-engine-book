#version 330 core

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

layout(location = 0) in  vec3 a_pos;
layout(location = 3) in  vec3 a_normal;

out vec3 v_normal;
out vec3 v_frag_pos;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0);

    v_normal = a_normal;
    v_frag_pos = vec3(u_model * vec4(a_pos, 1.0));
}