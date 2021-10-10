#version 330 core

uniform sampler2D u_diffuse_texture;

in vec4 v_color;
in vec2 v_uv;
layout(location = 0) out vec4 o_fragColor;
void main()
{
    o_fragColor = texture(u_diffuse_texture,v_uv) * v_color;
}