//
// Created by captain on 6/19/2022.
//

#ifndef UBO_UNIFORM_BUFFER_OBJECT_MANAGER_H
#define UBO_UNIFORM_BUFFER_OBJECT_MANAGER_H

#include <unordered_map>
#include <vector>
#include <string>
#include "glm/glm.hpp"

/// Uniform Block <==> Binding Point <==> Uniform Buffer Object
class UniformBlockInstanceBindingInfo{
public:
    std::string uniform_block_instance_name_;
    std::string uniform_block_name_;
    unsigned short uniform_block_size_;
    unsigned int binding_point_;
    unsigned int uniform_buffer_object_;
};

enum UniformBlockMemberType{
    FLOAT_TYPE,
    VEC4_TYPE,
    MAT3_TYPE,
    MAT4_TYPE
};

/// Uniform Block 成员变量信息
class UniformBlockMember{
public:
    std::string member_name_;
    int offset_;
    short data_size_;
};

/// Uniform Block 结构信息
class UniformBlock{
public:
    std::vector<UniformBlockMember> uniform_block_member_vec_;
};

class UniformBufferObjectManager {
public:
    /// 返回所有Shader的uniform block信息。
    /// \return
    static std::vector<UniformBlockInstanceBindingInfo>& UniformBlockInstanceBindingInfoArray(){
        return kUniformBlockInstanceBindingInfoArray;
    }

    /// 返回所有Uniform block结构。
    /// \return
    static std::unordered_map<std::string,UniformBlock>& UniformBlockMap(){
        return kUniformBlockMap;
    }

    static void Init();

    /// 初始化UBO
    static void CreateUniformBufferObject();

    /// 更新UBO数据(float)
    /// \param uniform_block_instance_name
    /// \param uniform_block_member_name
    /// \param value
    static void UpdateUniformBlockSubData1f(std::string uniform_block_instance_name, std::string uniform_block_member_name, float value);

    /// 更新UBO数据(vec3)
    /// \param uniform_block_instance_name
    /// \param uniform_block_member_name
    /// \param value
    static void UpdateUniformBlockSubData3f(std::string uniform_block_instance_name, std::string uniform_block_member_name, glm::vec3& value);

    /// 更新UBO数据(int)
    /// \param uniform_block_instance_name
    /// \param uniform_block_member_name
    /// \param value
    static void UpdateUniformBlockSubData1i(std::string uniform_block_instance_name, std::string uniform_block_member_name, int value);

private:
    static std::vector<UniformBlockInstanceBindingInfo> kUniformBlockInstanceBindingInfoArray;//统计所有Shader的uniform block信息。

    static std::unordered_map<std::string,UniformBlock> kUniformBlockMap;//映射Uniform block结构。
};


#endif //UBO_UNIFORM_BUFFER_OBJECT_MANAGER_H
