//
// Created by captain on 6/19/2022.
//

#include "uniform_buffer_object_manager.h"
#include <glad/gl.h>

std::vector<UniformBufferObjectManager::UniformBlock> UniformBufferObjectManager::kUniformBlockArray={
        {"Ambient",16,0,0},
        {"Light",28,1,0}
};

void UniformBufferObjectManager::CreateUniformBufferObject(){
    for (int i = 0; i < kUniformBlockArray.size(); ++i) {
        glGenBuffers(1, &kUniformBlockArray[i].uniform_buffer_object_);
        glBindBuffer(GL_UNIFORM_BUFFER, kUniformBlockArray[i].uniform_buffer_object_);
        //先不填数据
        unsigned short uniform_block_data_size=kUniformBlockArray[i].uniform_block_size_;
        glBufferData(GL_UNIFORM_BUFFER, uniform_block_data_size, NULL, GL_DYNAMIC_DRAW);
        //串联 UBO 和 binding point 绑定
        glBindBufferBase(GL_UNIFORM_BUFFER, kUniformBlockArray[i].binding_point_, kUniformBlockArray[i].uniform_buffer_object_);
    }
}