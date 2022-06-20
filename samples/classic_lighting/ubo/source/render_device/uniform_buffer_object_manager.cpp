//
// Created by captain on 6/19/2022.
//

#include "uniform_buffer_object_manager.h"
#include <glad/gl.h>
#include "utils/debug.h"

std::vector<UniformBlockBindingInfo> UniformBufferObjectManager::kUniformBlockBindingInfoArray={
        {"Ambient",16,0,0},
        {"Light",28,1,0}
};

glm::vec3 light_color(1.0,1.0,1.0);
float light_intensity=0.3;

glm::vec3 pos(0,0,0);
glm::vec3 color(1.0,1.0,1.0);
float intensity=1.0;

std::unordered_map<std::string,UniformBlock> UniformBufferObjectManager::kUniformBlockMap={
        {"Ambient",{
            {
                        {"light_color",0,sizeof(glm::vec3), &light_color[0]},
                        {"light_intensity",16,sizeof(float), &light_intensity}
                }
         }},
         {"Light",{
                {
                        {"pos",0,sizeof(glm::vec3), &pos[0]},
                        {"color",16,sizeof(glm::vec3), &color[0]},
                        {"intensity",32,sizeof(float), &intensity}
                }
         }}
};

void UniformBufferObjectManager::CreateUniformBufferObject(){
    for (int i = 0; i < kUniformBlockBindingInfoArray.size(); ++i) {
        glGenBuffers(1, &kUniformBlockBindingInfoArray[i].uniform_buffer_object_);__CHECK_GL_ERROR__
        glBindBuffer(GL_UNIFORM_BUFFER, kUniformBlockBindingInfoArray[i].uniform_buffer_object_);__CHECK_GL_ERROR__
        //先不填数据
        unsigned short uniform_block_data_size=kUniformBlockBindingInfoArray[i].uniform_block_size_;
        glBufferData(GL_UNIFORM_BUFFER, uniform_block_data_size, NULL, GL_DYNAMIC_DRAW);__CHECK_GL_ERROR__

        UniformBlock uniform_block=kUniformBlockMap[kUniformBlockBindingInfoArray[i].uniform_block_name_];
        for (auto& uniform_block_member:uniform_block.uniform_block_member_vec_) {
            glBufferSubData(GL_UNIFORM_BUFFER, 0, uniform_block_member.data_size_, uniform_block_member.data_);__CHECK_GL_ERROR__
        }

        //串联 UBO 和 binding point 绑定
        glBindBufferBase(GL_UNIFORM_BUFFER, kUniformBlockBindingInfoArray[i].binding_point_, kUniformBlockBindingInfoArray[i].uniform_buffer_object_);__CHECK_GL_ERROR__
    }
}