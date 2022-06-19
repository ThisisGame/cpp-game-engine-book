//
// Created by captain on 6/19/2022.
//

#ifndef UBO_UNIFORM_BUFFER_OBJECT_MANAGER_H
#define UBO_UNIFORM_BUFFER_OBJECT_MANAGER_H

#include <map>
#include <vector>
#include <string>


class UniformBufferObjectManager {
public:
    /// 初始化UBO
    static void CreateUniformBufferObject();

    class UniformBlock{
    public:
        std::string uniform_block_name_;
        unsigned short uniform_block_size_;
        unsigned int binding_point_;
        unsigned int uniform_buffer_object_;
    };

    static std::vector<UniformBlock>& UniformBlockArray(){
        return kUniformBlockArray;
    }

private:
    static std::vector<UniformBlock> kUniformBlockArray;//所有Shader的uniform block统计。
};


#endif //UBO_UNIFORM_BUFFER_OBJECT_MANAGER_H
