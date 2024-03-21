#version 330 core

//环境光
struct Ambient {
    vec3  color;//环境光 alignment:12 offset:0
    float intensity;//环境光强度 alignment:4 offset:16
};

layout(std140) uniform AmbientBlock {
    Ambient data;
}u_ambient;

//方向光
struct DirectionalLight {
    vec3  dir;//方向 alignment:12 offset:0
    vec3  color;//颜色 alignment:12 offset:16
    float intensity;//强度 alignment:4 offset:28
};

#define DIRECTIONAL_LIGHT_MAX_NUM 128

layout(std140) uniform DirectionalLightBlock {
    DirectionalLight data[DIRECTIONAL_LIGHT_MAX_NUM];
    int actually_used_count;//实际创建的灯光数量
}u_directional_light_array;

//点光
struct PointLight {
    vec3  pos;//位置 alignment:16 offset:0
    vec3  color;//颜色 alignment:12 offset:16
    float intensity;//强度 alignment:4 offset:28

    float constant;//点光衰减常数项 alignment:4 offset:32
    float linear;//点光衰减一次项 alignment:4 offset:36
    float quadratic;//点光衰减二次项 alignment:4 offset:40
};

#define POINT_LIGHT_MAX_NUM 128

//灯光数组
layout(std140) uniform PointLightBlock {
    PointLight data[POINT_LIGHT_MAX_NUM];
    int actually_used_count;//实际创建的灯光数量
}u_point_light_array;

uniform vec3 u_view_pos;

uniform sampler2D u_frag_position_texture;//顶点片段坐标纹理
uniform sampler2D u_frag_normal_texture;//顶点片段法线纹理
uniform sampler2D u_frag_vertex_color_texture;//顶点片段顶点颜色纹理
uniform sampler2D u_frag_diffuse_color_texture;//顶点片段Diffuse纹理
uniform sampler2D u_frag_specular_intensity_texture;//顶点片段高光强度纹理
uniform sampler2D u_frag_specular_highlight_shininess_texture;//顶点片段反光度纹理
uniform sampler2D u_ssao_texture;//SSAO纹理

in vec2 v_uv;

layout(location = 0) out vec4 o_fragColor;

void main()
{
	vec3 frag_position = texture(u_frag_position_texture,v_uv).rgb;
	vec3 frag_normal = texture(u_frag_normal_texture,v_uv).rgb;
	vec3 frag_vertex_color = texture(u_frag_vertex_color_texture,v_uv).rgb;
	vec3 frag_diffuse_color = texture(u_frag_diffuse_color_texture,v_uv).rgb;
	float frag_specular_intensity = texture(u_frag_specular_intensity_texture,v_uv).r;
	float frag_specular_highlight_shininess = texture(u_frag_specular_highlight_shininess_texture,v_uv).r;
    float ssao = texture(u_ssao_texture,v_uv).r;
	
    //ambient
    vec3 ambient_color = u_ambient.data.color * u_ambient.data.intensity * frag_diffuse_color;
	
    vec3 total_diffuse_color=vec3(0.0,0.0,0.0);
    vec3 total_specular_color=vec3(0.0,0.0,0.0);

    //directional light
    for(int i=0;i<u_directional_light_array.actually_used_count;i++){
        DirectionalLight directional_light=u_directional_light_array.data[i];

        //diffuse 计算漫反射光照
        vec3 normal=normalize(frag_normal);
        vec3 light_dir=normalize(-directional_light.dir);
        float diffuse_intensity = max(dot(normal,light_dir),0.0);
        vec3 diffuse_color = directional_light.color * diffuse_intensity * directional_light.intensity * frag_diffuse_color;

        //specular 计算高光
        vec3 reflect_dir=reflect(-light_dir,frag_normal);
        vec3 view_dir=normalize(u_view_pos-frag_position);
        float spec=pow(max(dot(view_dir,reflect_dir),0.0),frag_specular_highlight_shininess);
        vec3 specular_color = directional_light.color * spec * directional_light.intensity * frag_diffuse_color;

        //将每一个方向光的计算结果叠加
        total_diffuse_color=total_diffuse_color+diffuse_color;
        total_specular_color=total_specular_color+specular_color;
    }

    //point light
    for(int i=0;i<u_point_light_array.actually_used_count;i++){
        PointLight point_light=u_point_light_array.data[i];

        //diffuse 计算漫反射光照
        vec3 normal=normalize(frag_normal);
        vec3 light_dir=normalize(point_light.pos - frag_position);
        float diffuse_intensity = max(dot(normal,light_dir),0.0);
        vec3 diffuse_color = point_light.color * diffuse_intensity * point_light.intensity * frag_diffuse_color;

        //specular 计算高光
        vec3 reflect_dir=reflect(-light_dir,frag_normal);
        vec3 view_dir=normalize(u_view_pos-frag_position);
        float spec=pow(max(dot(view_dir,reflect_dir),0.0),frag_specular_highlight_shininess);
        float specular_highlight_intensity = frag_specular_intensity;//从纹理中获取高光强度
        vec3 specular_color = point_light.color * spec * specular_highlight_intensity * frag_diffuse_color.rgb;

        //attenuation 计算点光源衰减值
        float distance=length(point_light.pos - frag_position);
        float attenuation = 1.0 / (point_light.constant + point_light.linear * distance + point_light.quadratic * (distance * distance));

        //将每一个点光源的计算结果叠加
        total_diffuse_color=total_diffuse_color+diffuse_color*attenuation;
        total_specular_color=total_specular_color+specular_color*attenuation;
    }

    o_fragColor = vec4(ambient_color + total_diffuse_color + total_specular_color,1.0);

//    o_fragColor = vec4(1.0,0.0,0.0,1.0);
}