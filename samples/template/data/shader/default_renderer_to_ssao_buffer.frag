#version 330 core

uniform sampler2D u_frag_position_texture;//顶点片段坐标纹理
uniform sampler2D u_frag_normal_texture;//顶点片段法线纹理
uniform sampler2D u_noise_texture;//噪声纹理
uniform vec3 u_ssao_kernel[64];//ssao采样核心

uniform vec2 u_noise_texture_size;//噪声纹理大小
uniform vec2 u_screen_size;//屏幕大小

uniform mat4 u_projection;

in vec2 v_uv;

layout(location = 0) out vec4 o_fragColor;


int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

void main()
{
	vec3 frag_position = texture(u_frag_position_texture,v_uv).rgb;
	vec3 frag_normal = texture(u_frag_normal_texture,v_uv).rgb;

    //与屏幕像素1:1采样噪声纹理
    vec2 uv_scale=vec2(u_screen_size.x/u_noise_texture_size.x,u_screen_size.y/u_noise_texture_size.y);
    vec2 noise_uv = v_uv * uv_scale;
    vec3 random_noise = texture(u_noise_texture,noise_uv).rgb;

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(random_noise - frag_normal * dot(random_noise, frag_normal));
    vec3 bitangent = cross(frag_normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, frag_normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * u_ssao_kernel[i]; // from tangent to view-space
        samplePos = frag_position + samplePos * radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = u_projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // get sample depth
        float sampleDepth = texture(u_frag_position_texture, offset.xy).z; // get depth value of kernel sample

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(frag_position.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);

    o_fragColor = vec4(occlusion, occlusion, occlusion, 1.0);
}