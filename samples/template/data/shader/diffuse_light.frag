#version 330 core

uniform sampler2D u_diffuse_texture;//颜色纹理
uniform vec3 u_ambient_light_color;//环境光
uniform float u_ambient_light_intensity;//环境光强度
uniform vec3 u_light_pos;
uniform vec3 u_light_color;
uniform float u_light_intensity;

in vec4 v_color;//顶点色
in vec2 v_uv;
in vec3 v_normal;
in vec3 v_frag_pos;

layout(location = 0) out vec4 o_fragColor;
void main()
{
    //ambient
    vec3 ambient_color = u_ambient_light_color * u_ambient_light_intensity * texture(u_diffuse_texture,v_uv).rgb;

    //diffuse
    vec3 normal=normalize(v_normal);
    vec3 light_dir=normalize(u_light_pos - v_frag_pos);
    float diffuse_intensity = max(dot(normal,light_dir),0.0);
    vec3 diffuse_color = u_light_color * diffuse_intensity * u_light_intensity * texture(u_diffuse_texture,v_uv).rgb;

    o_fragColor = vec4(ambient_color + diffuse_color,1.0);
}