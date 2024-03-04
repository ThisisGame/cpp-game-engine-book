#version 330 core

uniform sampler2D u_diffuse_texture;//颜色纹理
uniform sampler2D u_specular_texture;//高光纹理

uniform float u_specular_highlight_shininess;//物体反光度，越高反光能力越强，高光点越小。

in vec4 v_color;//顶点色
in vec2 v_uv;
in vec3 v_normal;
in vec3 v_frag_pos;

layout(location = 0) out vec4 o_frag_position;
layout(location = 1) out vec4 o_frag_normal;
layout(location = 2) out vec4 o_frag_vertex_color;
layout(location = 3) out vec4 o_frag_diffuse_color;
layout(location = 4) out vec4 o_frag_specular_intensity;
layout(location = 5) out vec4 o_frag_specular_highlight_shininess;

void main()
{
	o_frag_position = vec4(v_frag_pos,1.0);
	o_frag_normal = vec4(v_normal,1.0);
	o_frag_vertex_color = v_color;
	o_frag_diffuse_color = texture(u_diffuse_texture,v_uv);
	o_frag_specular_intensity = texture(u_specular_texture,v_uv);
	o_frag_specular_highlight_shininess = vec4(vec3(u_specular_highlight_shininess),1.0);
}