//
// Created by captain on 6/19/2022.
//

#include "uniform_buffer_object_manager.h"
#include <glad/gl.h>
#include "utils/debug.h"

std::vector<UniformBlockBindingInfo> UniformBufferObjectManager::kUniformBlockBindingInfoArray={
        {"Ambient",16,0,0},
//        {"Light",28,1,0}
};

glm::vec3 light_color(1.0,0.0,0.0);
float light_intensity=0.3;

glm::vec3 pos(0,0,0);
glm::vec3 color(1.0,1.0,1.0);
float intensity=1.0;

std::unordered_map<std::string,UniformBlock> UniformBufferObjectManager::kUniformBlockMap={
        {"Ambient",{
            {
                    {"u_ambient_light_color",0,sizeof(glm::vec3), &light_color[0]},
                    {"u_ambient_light_intensity",12,sizeof(float), &light_intensity}
                }
         }},
//         {"Light",{
//                {
//                        {"pos",0,sizeof(glm::vec3), &pos[0]},
//                        {"color",16,sizeof(glm::vec3), &color[0]},
//                        {"intensity",32,sizeof(float), &intensity}
//                }
//         }}
};

void UniformBufferObjectManager::CreateUniformBufferObject(){
    for (int i = 0; i < kUniformBlockBindingInfoArray.size(); ++i) {
        UniformBlockBindingInfo& uniform_block_binding_info=kUniformBlockBindingInfoArray[i];
        glGenBuffers(1, &uniform_block_binding_info.uniform_buffer_object_);__CHECK_GL_ERROR__
        glBindBuffer(GL_UNIFORM_BUFFER, uniform_block_binding_info.uniform_buffer_object_);__CHECK_GL_ERROR__
        //先不填数据
        unsigned short uniform_block_data_size=uniform_block_binding_info.uniform_block_size_;
        glBufferData(GL_UNIFORM_BUFFER, uniform_block_data_size, NULL, GL_STATIC_DRAW);__CHECK_GL_ERROR__

        UniformBlock uniform_block=kUniformBlockMap[uniform_block_binding_info.uniform_block_name_];
        for (auto& uniform_block_member:uniform_block.uniform_block_member_vec_) {
            glBufferSubData(GL_UNIFORM_BUFFER, uniform_block_member.offset_, uniform_block_member.data_size_, uniform_block_member.data_);__CHECK_GL_ERROR__
        }

        //串联 UBO 和 binding point 绑定
        glBindBufferBase(GL_UNIFORM_BUFFER, uniform_block_binding_info.binding_point_, uniform_block_binding_info.uniform_buffer_object_);__CHECK_GL_ERROR__
    }
}