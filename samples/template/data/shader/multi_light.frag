#version 330 core

uniform sampler2D u_diffuse_texture;//颜色纹理

//环境光
struct Ambient {
    vec3  color;//环境光 alignment:12 offset:0
    float intensity;//环境光强度 alignment:4 offset:16
};

layout(std140) uniform AmbientBlock {
    Ambient data;
}u_ambient;

//点光
struct PointLight {
    vec3  pos;//位置 alignment:16 offset:0
    vec3  color;//颜色 alignment:12 offset:16
    float intensity;//强度 alignment:4 offset:28

    float constant;//点光衰减常数项 alignment:4 offset:32
    float linear;//点光衰减一次项 alignment:4 offset:36
    float quadratic;//点光衰减二次项 alignment:4 offset:40
};

#define MAX_LIGHT_NUM 2

//灯光数组
layout(std140) uniform PointLightBlock {
    PointLight data[MAX_LIGHT_NUM];
}u_point_light_array;

uniform vec3 u_view_pos;
//uniform float u_specular_highlight_intensity;//镜面高光强度
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
    vec3 total_diffuse_color;
    vec3 total_specular_color;

    for(int i=0;i<MAX_LIGHT_NUM;i++){
        //diffuse
        vec3 normal=normalize(v_normal);
        vec3 light_dir=normalize(u_point_light_array.data[i].pos - v_frag_pos);
        float diffuse_intensity = max(dot(normal,light_dir),0.0);
        vec3 diffuse_color = u_point_light_array.data[i].color * diffuse_intensity * u_point_light_array.data[i].intensity * texture(u_diffuse_texture,v_uv).rgb;

        //specular
        vec3 reflect_dir=reflect(-light_dir,v_normal);
        vec3 view_dir=normalize(u_view_pos-v_frag_pos);
        float spec=pow(max(dot(view_dir,reflect_dir),0.0),u_specular_highlight_shininess);
        float specular_highlight_intensity = texture(u_specular_texture,v_uv).r;//从纹理中获取高光强度
        vec3 specular_color = u_point_light_array.data[i].color * spec * specular_highlight_intensity * texture(u_diffuse_texture,v_uv).rgb;

        // attenuation
        float distance=length(u_point_light_array.data[i].pos - v_frag_pos);
        float attenuation = 1.0 / (u_point_light_array.data[i].constant + u_point_light_array.data[i].linear * distance + u_point_light_array.data[i].quadratic * (distance * distance));

        total_diffuse_color=total_diffuse_color+diffuse_color*attenuation;
        total_specular_color=total_specular_color+specular_color*attenuation;
    }

    o_fragColor = vec4(ambient_color + total_diffuse_color + total_specular_color,1.0);
}