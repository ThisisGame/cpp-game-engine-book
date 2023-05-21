#version 330 core

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

uniform mat4 u_shadow_camera_view;
uniform mat4 u_shadow_camera_projection;

layout(location = 0) in  vec3 a_pos;
layout(location = 1) in  vec4 a_color;
layout(location = 2) in  vec2 a_uv;

out vec4 v_color;
out vec2 v_uv;

out vec4 v_shadow_camera_gl_Position;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0);
    v_color = a_color;
    v_uv = a_uv;

    v_shadow_camera_gl_Position=u_shadow_camera_projection * u_shadow_camera_view * u_model * vec4(a_pos, 1.0);
}