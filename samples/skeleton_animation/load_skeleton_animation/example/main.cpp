#define GLFW_INCLUDE_NONE

#include <iostream>
#include "utils/debug.h"
#include "lua_binding/lua_binding.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast_beauty.hpp>

int main(void){
//    //设置lua搜索目录
//    LuaBinding::Init(";..\\example\\?.lua;");
//    //绑定引擎所有类到Lua
//    LuaBinding::BindLua();
//    //执行lua
//    LuaBinding::RunLuaFile("../example/main.lua");
//    //调用lua main()
//    LuaBinding::CallLuaFunction("main");

    //blender的位移在右边一列
//    glm::mat4 mat=glm::mat4(1.000000, 0.000000, 0.000000, 0.000000,
//                            0.000000, 0.000000, -1.000000, 0.000000,
//                            0.000000, 1.000000, 0.000000, 2.000000,
//                            0.000000, 0.000000, 0.000000, 1.000000);
//
//    std::cout << glm::to_string(glm::transpose(mat)) << std::endl;

//    glm::mat4 trans=glm::translate(glm::vec3(0.0f,2.0f,0.0f)); //glm的位移在底下一行。
//    std::cout << glm::to_string(trans) << std::endl;

//    glm::mat4 mat=glm::rotate(glm::mat4(1.0f),glm::radians(30.0f),glm::vec3(0.0f,0.0f,1.0f));



    glm::mat4 mat=glm::translate(glm::vec3(4.f,5.f,6.f));
    std::cout<<glm::to_string_beauty(mat)<<std::endl;

    glm::vec4 pos=glm::vec4(7.f,8.f,9.f,1.f);
    std::cout<<glm::to_string(pos)<<std::endl;

    std::cout<<glm::to_string(  mat * pos  )<<std::endl;


//    glm::vec4 bone_tail_joint_vec4_local = glm::vec4(0.0000, 2.0000,  0.0000,1.000);
//
//    //Bone head在当前k帧的变换
//    glm::mat4 bone_head_joint_mat_basis = glm::mat4 (0.8660, -0.5000, 0.0000, 0.0000,
//                                                     0.5000, 0.8660, 0.0000, 0.0000,
//                                                     0.0000, 0.0000, 1.0000, 0.0000,
//                                                     0.0000, 0.0000, 0.0000, 1.0000);
//
//    std::cout << glm::to_string(bone_head_joint_mat_basis*bone_tail_joint_vec4_local) << std::endl;

    return 0;
}


