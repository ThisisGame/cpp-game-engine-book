//
// Created by captain on 6/19/2022.
//

#include "uniform_buffer_object_manager.h"
#include <glad/gl.h>
#include "utils/debug.h"
#include "render_task_type.h"
#include "render_task_queue.h"
#include "render_task_producer.h"

#define MAX_LIGHT_NUM 2 //最大灯光数量

std::vector<UniformBlockBindingInfo> UniformBufferObjectManager::kUniformBlockBindingInfoArray={
        {"Ambient",16,0,0},
        {"MultiLight",48*2,1,0}
//        {"MultiLight",44,1,0}
};

std::unordered_map<std::string,UniformBlock> UniformBufferObjectManager::kUniformBlockMap;

void UniformBufferObjectManager::Init(){
    kUniformBlockMap["Ambient"]={
            {
                    {"u_ambient_light_color",0,sizeof(glm::vec3), nullptr},
                    {"u_ambient_light_intensity",12,sizeof(float), nullptr}
            }
    };
    kUniformBlockMap["MultiLight"]={{}};
    for(int i=0;i<MAX_LIGHT_NUM;i++){
        std::vector<UniformBlockMember>& uniform_block_member_vec=kUniformBlockMap["MultiLight"].uniform_block_member_vec_;
        uniform_block_member_vec.push_back({fmt::format("u_light_array[{}].u_light_pos",i),48*i+0,sizeof(glm::vec3), nullptr});
        uniform_block_member_vec.push_back({fmt::format("u_light_array[{}].u_light_color",i),48*i+16,sizeof(glm::vec3), nullptr});
        uniform_block_member_vec.push_back({fmt::format("u_light_array[{}].u_light_intensity",i),48*i+28,sizeof(float), nullptr});
        uniform_block_member_vec.push_back({fmt::format("u_light_array[{}].u_light_constant",i),48*i+32,sizeof(float), nullptr});
        uniform_block_member_vec.push_back({fmt::format("u_light_array[{}].u_light_linear",i),48*i+36,sizeof(float), nullptr});
        uniform_block_member_vec.push_back({fmt::format("u_light_array[{}].u_light_quadratic",i),48*i+40,sizeof(float), nullptr});
    }
}

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