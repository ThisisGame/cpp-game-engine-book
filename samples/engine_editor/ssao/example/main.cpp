#define GLFW_INCLUDE_NONE

#include <random>
#include "utils/debug.h"
#include "lua_binding/lua_binding.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//float lerp(float a, float b, float f)
//{
//    return a + f * (b - a);
//}

int main(void){

    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
    std::default_random_engine generator;
//    std::vector<glm::vec3> ssaoKernel;
//    for (unsigned int i = 0; i < 64; ++i)
//    {
//        glm::vec3 sample(
//                randomFloats(generator) * 2.0 - 1.0,
//                randomFloats(generator) * 2.0 - 1.0,
//                randomFloats(generator)
//        );
//        sample  = glm::normalize(sample);
//        sample *= randomFloats(generator);
//
//        float scale = (float)i / 64.0;
//        scale   = lerp(0.1f, 1.0f, scale * scale);
//        sample *= scale;
//        ssaoKernel.push_back(sample);
//    }

//    std::vector<glm::vec3> ssaoNoise;
//    for (unsigned int i = 0; i < 16; i++)
//    {
//        glm::vec3 noise(
//                randomFloats(generator) * 2.0 - 1.0,
//                randomFloats(generator) * 2.0 - 1.0,
//                0.0f);
//        ssaoNoise.push_back(noise);
//    }

    //设置lua搜索目录
    LuaBinding::Init(";../example/?.lua;../source_lua/?.lua;../source_lua/utils/?.lua;../source_lua/component/?.lua");
    //绑定引擎所有类到Lua
    LuaBinding::BindLua();
    //执行lua
    LuaBinding::RunLuaFile("../example/main.lua");
    //调用lua main()
    LuaBinding::CallLuaFunction("main");

    return 0;
}


