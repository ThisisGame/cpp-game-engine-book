#version 330 core

uniform sampler2D u_diffuse_texture;

in vec4 v_color;
in vec2 v_uv;
layout(location = 0) out vec4 o_fragColor;
void main()
{
    vec4 color = texture(u_diffuse_texture,v_uv) * v_color;
	if(color.a<=0.8)
	{
		discard;
	}
	o_fragColor = color;
}