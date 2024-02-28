#version 330 core

in vec4 v_color;//顶点色
in vec2 v_uv;
in vec3 v_normal;
in vec3 v_frag_pos;

layout(location = 0) out vec4 o_vertex_position;
layout(location = 1) out vec4 o_vertex_normal;
layout(location = 2) out vec4 o_vertex_color;
layout(location = 3) out vec4 o_uv_color;
void main()
{
	o_vertex_position = vec4(v_frag_pos,1.0);
	o_vertex_normal = vec4(v_normal,1.0);
	o_vertex_color = v_color;
	o_uv_color = vec4(v_uv,1.0,1.0);
}