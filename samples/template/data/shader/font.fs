#version 330

uniform sampler2D u_diffuse_texture;

in vec4 v_color;
in vec2 v_uv;
layout(location = 0) out vec4 o_fragColor;
void main()
{
    vec4 color=texture(u_diffuse_texture,v_uv);
    o_fragColor = vec4(1.0,0.0,0.0,color.a);
};