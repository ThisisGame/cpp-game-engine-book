//
// Created by captain on 6/19/2022.
//

#include "uniform_buffer_object_manager.h"
#include <glad/gl.h>
#include "utils/debug.h"
#include "render_task_type.h"
#include "render_task_queue.h"
#include "render_task_producer.h"

#define DIRECTIONAL_LIGHT_MAX_NUM 128 //最大方向光数量
#define POINT_LIGHT_MAX_NUM 128 //最大点光源数量

std::vector<UniformBlockInstanceBindingInfo> UniformBufferObjectManager::kUniformBlockInstanceBindingInfoArray={
        {"u_ambient","AmbientBlock",16,0,0},
        {"u_directional_light_array","DirectionalLightBlock",32*DIRECTIONAL_LIGHT_MAX_NUM+sizeof(int),1,0},
        {"u_point_light_array","PointLightBlock",48*POINT_LIGHT_MAX_NUM+sizeof(int),2,0}
};

std::unordered_map<std::string,UniformBlock> UniformBufferObjectManager::kUniformBlockMap;

void UniformBufferObjectManager::Init(){
    //环境光
    kUniformBlockMap["AmbientBlock"]={
            {
                    {"data.color",0,sizeof(glm::vec3)},
                    {"data.intensity",12,sizeof(float)}
            }
    };

    //方向光
    kUniformBlockMap["DirectionalLightBlock"]={{}};
    {
        std::vector<UniformBlockMember>& uniform_block_member_vec=kUniformBlockMap["DirectionalLightBlock"].uniform_block_member_vec_;
        for(int i=0;i<POINT_LIGHT_MAX_NUM;i++){
            uniform_block_member_vec.push_back({fmt::format("data[{}].dir",i),32*i+0,sizeof(glm::vec3)});
            uniform_block_member_vec.push_back({fmt::format("data[{}].color",i),32*i+16,sizeof(glm::vec3)});
            uniform_block_member_vec.push_back({fmt::format("data[{}].intensity",i),32*i+28,sizeof(float)});
        }
        uniform_block_member_vec.push_back({"actually_used_count",32*POINT_LIGHT_MAX_NUM,sizeof(int)});
    }

    //点光源数组
    kUniformBlockMap["PointLightBlock"]={{}};
    {
        std::vector<UniformBlockMember>& uniform_block_member_vec=kUniformBlockMap["PointLightBlock"].uniform_block_member_vec_;
        for(int i=0;i<POINT_LIGHT_MAX_NUM;i++){
            uniform_block_member_vec.push_back({fmt::format("data[{}].pos",i),48*i+0,sizeof(glm::vec3)});
            uniform_block_member_vec.push_back({fmt::format("data[{}].color",i),48*i+16,sizeof(glm::vec3)});
            uniform_block_member_vec.push_back({fmt::format("data[{}].intensity",i),48*i+28,sizeof(float)});
            uniform_block_member_vec.push_back({fmt::format("data[{}].constant",i),48*i+32,sizeof(float)});
            uniform_block_member_vec.push_back({fmt::format("data[{}].linear",i),48*i+36,sizeof(float)});
            uniform_block_member_vec.push_back({fmt::format("data[{}].quadratic",i),48*i+40,sizeof(float)});
        }
        uniform_block_member_vec.push_back({"actually_used_count",48*POINT_LIGHT_MAX_NUM,sizeof(int)});
    }
}

void UniformBufferObjectManager::CreateUniformBufferObject(){
    for (int i = 0; i < kUniformBlockInstanceBindingInfoArray.size(); ++i) {
        UniformBlockInstanceBindingInfo& uniform_block_binding_info=kUniformBlockInstanceBindingInfoArray[i];
        glGenBuffers(1, &uniform_block_binding_info.uniform_buffer_object_);__CHECK_GL_ERROR__
        glBindBuffer(GL_UNIFORM_BUFFER, uniform_block_binding_info.uniform_buffer_object_);__CHECK_GL_ERROR__
        //先不填数据
        unsigned short uniform_block_data_size=uniform_block_binding_info.uniform_block_size_;
        glBufferData(GL_UNIFORM_BUFFER, uniform_block_data_size, NULL, GL_STATIC_DRAW);__CHECK_GL_ERROR__

        //串联 UBO 和 binding point 绑定
        glBindBufferBase(GL_UNIFORM_BUFFER, uniform_block_binding_info.binding_point_, uniform_block_binding_info.uniform_buffer_object_);__CHECK_GL_ERROR__
    }
}

void UniformBufferObjectManager::UpdateUniformBlockSubData1f(std::string uniform_block_instance_name, std::string uniform_block_member_name, float value){
    void* data= malloc(sizeof(float));
    memcpy(data,&value,sizeof(float));
    RenderTaskProducer::ProduceRenderTaskUpdateUBOSubData(uniform_block_instance_name, std::move(uniform_block_member_name), data);
}

void UniformBufferObjectManager::UpdateUniformBlockSubData3f(std::string uniform_block_instance_name, std::string uniform_block_member_name, glm::vec3& value){
    void* data= malloc(sizeof(glm::vec3));
    memcpy(data,&value,sizeof(glm::vec3));
    RenderTaskProducer::ProduceRenderTaskUpdateUBOSubData(uniform_block_instance_name, uniform_block_member_name, data);
}

void UniformBufferObjectManager::UpdateUniformBlockSubData1i(std::string uniform_block_instance_name, std::string uniform_block_member_name, int value){
    void* data= malloc(sizeof(int));
    memcpy(data,&value,sizeof(int));
    RenderTaskProducer::ProduceRenderTaskUpdateUBOSubData(uniform_block_instance_name, std::move(uniform_block_member_name), data);
}