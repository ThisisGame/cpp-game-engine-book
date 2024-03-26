#version 330 core

//环境光
struct Ambient {
    vec3  color;//环境光 alignment:12 offset:0
    float intensity;//环境光强度 alignment:4 offset:16
};

layout(std140) uniform AmbientBlock {
    Ambient data;
}u_ambient;

uniform vec3 u_view_pos;

uniform sampler2D u_ssao_texture;//SSAO纹理

uniform float u_use_ssao;//是否使用SSAO

in vec2 v_uv;

layout(location = 0) out vec4 o_fragColor;

void main()
{
	vec3 frag_diffuse_color = vec3(0.9,0.9,0.9);
    float ssao = texture(u_ssao_texture,v_uv).r;

    if(u_use_ssao<0.5) {
        ssao = 1.0;
    }
	
    //ambient
    vec3 ambient_color = u_ambient.data.color * u_ambient.data.intensity * frag_diffuse_color * ssao;

    o_fragColor = vec4(ambient_color,1.0);
}