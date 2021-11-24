#define GLFW_INCLUDE_NONE

#include <iostream>
#include "utils/debug.h"
#include "lua_binding/lua_binding.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

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
    glm::mat4 mat=glm::mat4(1.000000, 0.000000, 0.000000, 0.000000,
                            0.000000, 0.000000, -1.000000, 0.000000,
                            0.000000, 1.000000, 0.000000, 2.000000,
                            0.000000, 0.000000, 0.000000, 1.000000);
    glm::mat4 trans=glm::translate(glm::vec3(0.0f,2.0f,0.0f)); //glm的位移在底下一行。
    std::cout << glm::to_string(trans) << std::endl;
    glm::mat4 rotate=glm::rotate(glm::mat4(1.0f),glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));
    std::cout << glm::to_string(rotate) << std::endl;

//    //整个骨架的变换 x=1
//    glm::mat4 mat_model = glm::mat4 (1.0000, 0.0000,  0.0000, 0.0000,
//                                     0.0000, 1.0000,  0.0000, 0.0000,
//                                     0.0000, 0.0000,  1.0000, 0.0000,
//                                     1.0000, 0.0000,  0.0000, 1.0000);
//
//    //Bone tail 在T-Pos下的变换
//    glm::mat4 bone_tail_joint_mat_local = glm::mat4 (1.0000, 0.0000,  0.0000, 0.0000,
//                                                     0.0000, 1.0000,  0.0000, 0.0000,
//                                                     0.0000, 0.0000,  1.0000, 0.0000,
//                                                     0.0000, 2.0000,  0.0000, 1.0000);
//
//    glm::vec4 bone_tail_joint_vec4_local = glm::vec4(0.0000, 2.0000,  0.0000,1.000);
//
//    //Bone head在当前k帧的变换
//    glm::mat4 bone_head_joint_mat_basis = glm::mat4 (0.8660, -0.5000, 0.0000, 0.0000,
//                                                     0.5000, 0.8660, 0.0000, 0.0000,
//                                                     0.0000, 0.0000, 1.0000, 0.0000,
//                                                     0.0000, 0.0000, 0.0000, 1.0000);
//
//    std::cout << glm::to_string(mat_model * bone_head_joint_mat_basis*bone_tail_joint_mat_local) << std::endl;
//    std::cout << glm::to_string(mat_model * bone_head_joint_mat_basis*bone_tail_joint_vec4_local) << std::endl;
//
//    //Bone_1 tail 在T-Pos下的变换
//    glm::mat4 bone_1_tail_joint_mat_local = glm::mat4 (1.0000, 0.0000,  0.0000, 0.0000,
//                                                       0.0000, 1.0000,  0.0000, 0.0000,
//                                                       0.0000, 0.0000,  1.0000, 0.0000,
//                                                       0.0000, 1.0000,  0.0000, 1.0000);
//
//    glm::vec4 bone_1_tail_joint_vec4_local = glm::vec4(0.0000, 1.0000,  0.0000,1.000);
//
//    //Bone_1 head在当前k帧的变换
//    glm::mat4 bone_1_head_joint_mat_basis = glm::mat4 (0.5000, -0.8660, 0.0000, 0.0000,
//                                                       0.8660, 0.5000, 0.0000, 0.0000,
//                                                       0.0000, 0.0000, 1.0000, 0.0000,
//                                                       0.0000, 0.0000, 0.0000, 1.0000);
//
//    std::cout << glm::to_string(mat_model * bone_head_joint_mat_basis*bone_tail_joint_mat_local*bone_1_head_joint_mat_basis*bone_1_tail_joint_vec4_local) << std::endl;

    return 0;
}


