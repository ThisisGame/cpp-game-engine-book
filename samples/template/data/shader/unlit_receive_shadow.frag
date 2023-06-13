#version 330 core

uniform sampler2D u_depth_texture;
uniform sampler2D u_diffuse_texture;

in vec4 v_color;
in vec2 v_uv;
in vec4 v_shadow_camera_gl_Position;

layout(location = 0) out vec4 o_fragColor;

float ShadowCalculation(vec4 shadow_camera_gl_Position)
{
    //首先，将传入参数中的x,y,z三个分量除以第四个分量w，得到投影坐标proj_coords。
    vec3 proj_coords = shadow_camera_gl_Position.xyz / shadow_camera_gl_Position.w;
    //将投影坐标映射到范围[0,1]
    proj_coords = proj_coords * 0.5 + 0.5;
    //从深度图纹理(u_depth_texture)中提取最近的深度closest_depth，这个深度代表着从光源位置到当前像素的最短距离。
    float closest_depth = texture(u_depth_texture, proj_coords.xy).r;
    //获取当前像素的深度current_depth，即当前像素与光源之间的距离
    float current_depth = proj_coords.z;
    //使用偏移
    float bias = 0.005;
    //如果这个距离大于最近距离closest_depth，则说明当前像素在阴影之中，返回1.0，否则返回0.0，代表没有阴影。
    float shadow = current_depth - bias > closest_depth  ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    float shadow = ShadowCalculation(v_shadow_camera_gl_Position);
    o_fragColor = texture(u_diffuse_texture,v_uv) * v_color * (1-shadow);
}