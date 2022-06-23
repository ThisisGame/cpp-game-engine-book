//
// Created by captain on 6/19/2022.
//

#include "uniform_buffer_object_manager.h"
#include <glad/gl.h>
#include "utils/debug.h"
#include "render_task_type.h"
#include "render_task_queue.h"
#include "render_task_producer.h"

std::vector<UniformBlockBindingInfo> UniformBufferObjectManager::kUniformBlockBindingInfoArray={
        {"Ambient",16,0,0},
        {"Light",32,1,0}
};

std::unordered_map<std::string,UniformBlock> UniformBufferObjectManager::kUniformBlockMap={
        {"Ambient",{
            {
                    {"u_ambient_light_color",0,sizeof(glm::vec3), nullptr},
                    {"u_ambient_light_intensity",12,sizeof(float), nullptr}
                }
         }},
         {"Light",{
                {
                        {"u_light_pos",0,sizeof(glm::vec3), nullptr},
                        {"u_light_color",16,sizeof(glm::vec3), nullptr},
                        {"u_light_intensity",28,sizeof(float), nullptr}
                }
         }}
};

void UniformBufferObjectManager::CreateUniformBufferObject(){
    for (int i = 0; i < kUniformBlockBindingInfoArray.size(); ++i) {
        UniformBlockBindingInfo& uniform_block_binding_info=kUniformBlockBindingInfoArray[i];
        glGenBuffers(1, &uniform_block_binding_info.uniform_buffer_object_);__CHECK_GL_ERROR__
        glBindBuffer(GL_UNIFORM_BUFFER, uniform_block_binding_info.uniform_buffer_object_);__CHECK_GL_ERROR__
        //先不填数据
        unsigned short uniform_block_data_size=uniform_block_binding_info.uniform_block_size_;
        glBufferData(GL_UNIFORM_BUFFER, uniform_block_data_size, NULL, GL_STATIC_DRAW);__CHECK_GL_ERROR__

        //串联 UBO 和 binding point 绑定
        glBindBufferBase(GL_UNIFORM_BUFFER, uniform_block_binding_info.binding_point_, uniform_block_binding_info.uniform_buffer_object_);__CHECK_GL_ERROR__
    }
}

void UniformBufferObjectManager::UpdateUniformBlockSubData1f(std::string uniform_block_name, std::string uniform_block_member_name, float value){
    void* data= malloc(sizeof(float));
    memcpy(data,&value,sizeof(float));
    RenderTaskProducer::ProduceRenderTaskUpdateUBOSubData(uniform_block_name,uniform_block_member_name,data);
}

void UniformBufferObjectManager::UpdateUniformBlockSubData3f(std::string uniform_block_name, std::string uniform_block_member_name, glm::vec3& value){
    void* data= malloc(sizeof(glm::vec3));
    memcpy(data,&value,sizeof(glm::vec3));
    RenderTaskProducer::ProduceRenderTaskUpdateUBOSubData(uniform_block_name,uniform_block_member_name,data);
}