#version 330 core

uniform sampler2D u_diffuse_texture;//颜色纹理
uniform vec3 u_ambient_light_color;//环境光
uniform float u_ambient_light_intensity;//环境光强度

in vec4 v_color;//顶点色
in vec2 v_uv;

layout(location = 0) out vec4 o_fragColor;
void main()
{
    o_fragColor = texture(u_diffuse_texture,v_uv) * vec4(u_ambient_light_color,1.0) * u_ambient_light_intensity;
    o_fragColor.a=1.0;
}