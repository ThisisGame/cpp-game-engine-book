//
// Created by captain on 6/19/2022.
//

#ifndef UBO_UNIFORM_BUFFER_OBJECT_MANAGER_H
#define UBO_UNIFORM_BUFFER_OBJECT_MANAGER_H

#include <unordered_map>
#include <vector>
#include <string>
#include <glm/glm.hpp>

/// Uniform Block <==> Binding Point <==> Uniform Buffer Object
class UniformBlockBindingInfo{
public:
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
    void* data_;
};

/// Uniform Block 结构信息
class UniformBlock{
public:
    std::vector<UniformBlockMember> uniform_block_member_vec_;
};

class UniformBufferObjectManager {
public:
    static std::vector<UniformBlockBindingInfo>& UniformBlockBindingInfoArray(){
        return kUniformBlockBindingInfoArray;
    }

    /// 初始化UBO
    static void CreateUniformBufferObject();

    static void UpdateUniformBufferSubData(std::string& uniform_block_name,std::string& uniform_block_member_name,void* data);

    static void SetUniform1f(const std::string& uniform_block_name,std::string& uniform_block_member_name,float value);
    static void SetUniform3f(const std::string& uniform_block_name,std::string& uniform_block_member_name,glm::vec3& value);

private:
    static std::vector<UniformBlockBindingInfo> kUniformBlockBindingInfoArray;//所有Shader的uniform block统计。

    static std::unordered_map<std::string,UniformBlock> kUniformBlockMap;//映射Uniform block结构。
};


#endif //UBO_UNIFORM_BUFFER_OBJECT_MANAGER_H
