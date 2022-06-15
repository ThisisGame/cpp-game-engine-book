#version 330 core

uniform sampler2D u_diffuse_texture;//颜色纹理

//环境光
struct Ambient {
    vec3  light_color;//环境光
    float light_intensity;//环境光强度
};
uniform Ambient u_ambient;

//灯光
struct Light {
    vec3  pos;
    vec3  color;
    float intensity;
};
uniform Light u_light;

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
    vec3 ambient_color = u_ambient.light_color * u_ambient.light_intensity * texture(u_diffuse_texture,v_uv).rgb;

    //diffuse
    vec3 normal=normalize(v_normal);
    vec3 light_dir=normalize(u_light.pos - v_frag_pos);
    float diffuse_intensity = max(dot(normal,light_dir),0.0);
    vec3 diffuse_color = u_light.color * diffuse_intensity * u_light.intensity * texture(u_diffuse_texture,v_uv).rgb;

	//specular
	vec3 reflect_dir=reflect(-light_dir,v_normal);
	vec3 view_dir=normalize(u_view_pos-v_frag_pos);
	float spec=pow(max(dot(view_dir,reflect_dir),0.0),u_specular_highlight_shininess);
    float specular_highlight_intensity = texture(u_specular_texture,v_uv).r;//从纹理中获取高光强度
	vec3 specular_color = u_light.color * spec * specular_highlight_intensity * texture(u_diffuse_texture,v_uv).rgb;

    o_fragColor = vec4(ambient_color + diffuse_color + specular_color,1.0);
}