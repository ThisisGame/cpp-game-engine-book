#version 330 core

uniform sampler2D u_diffuse_texture;//颜色纹理

struct Ambient{
    vec3  color;//环境光 alignment:12 offset:0
    float intensity;//环境光强度 alignment:4 offset:12
};

//环境光
layout(std140) uniform AmbientBlock {
    Ambient data;
}u_ambient;

struct DirectionalLight {
    vec3  dir;//方向 alignment:12 offset:0
    vec3  color;//颜色 alignment:12 offset:16
    float intensity;//强度 alignment:4 offset:28
};

//灯光
layout(std140) uniform DirectionalLightBlock {
    DirectionalLight data;
}u_directional_light;

uniform vec3 u_view_pos;
uniform sampler2D u_specular_texture;//颜色纹理
uniform float u_specular_highlight_shininess;//物体反光度，越高反光能力越强，高光点越小。

in vec4 v_color;//顶点色
in vec2 v_uv;
in vec3 v_normal;
in vec3 v_frag_pos;

layout(location = 0) out vec4 o_fragColor;
void main()
{
    //ambient
    vec3 ambient_color = u_ambient.data.color * u_ambient.data.intensity * texture(u_diffuse_texture,v_uv).rgb;

    //diffuse
    vec3 normal=normalize(v_normal);
    vec3 light_dir=normalize(u_directional_light.data.dir);
    float diffuse_intensity = max(dot(normal,-light_dir),0.0);
    vec3 diffuse_color = u_directional_light.data.color * diffuse_intensity * u_directional_light.data.intensity * texture(u_diffuse_texture,v_uv).rgb;

    //specular
    vec3 reflect_dir=reflect(light_dir,v_normal);
    vec3 view_dir=normalize(u_view_pos-v_frag_pos);
    float spec=pow(max(dot(view_dir,reflect_dir),0.0),u_specular_highlight_shininess);
    float specular_highlight_intensity = texture(u_specular_texture,v_uv).r;//从纹理中获取高光强度
    vec3 specular_color = u_directional_light.data.color * spec * specular_highlight_intensity * texture(u_diffuse_texture,v_uv).rgb;

    o_fragColor = vec4(ambient_color + diffuse_color + specular_color,1.0);
}