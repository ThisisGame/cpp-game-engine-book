//
// Created by captainchen on 2021/5/11.
//

#include "mesh_renderer.h"
#include <glad/gl.h>
#include <rttr/registration>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "timetool/stopwatch.h"
#include "easy/profiler.h"
#include "material.h"
#include "mesh_filter.h"
#include "texture_2d.h"
#include "shader.h"
#include "camera.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "utils/debug.h"
#include "render_device/render_task_producer.h"
#include "render_device/gpu_resource_mapper.h"

using timetool::StopWatch;
using namespace rttr;
RTTR_REGISTRATION
{
    registration::class_<MeshRenderer>("MeshRenderer")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

MeshRenderer::MeshRenderer():Component(),material_(nullptr) {

}

MeshRenderer::~MeshRenderer() {

}

void MeshRenderer::SetMaterial(Material* material) {
    material_=material;
}


void MeshRenderer::Render() {
    EASY_FUNCTION(profiler::colors::Pink); // 标记函数
    //从当前Camera获取View Projection
    auto current_camera=Camera::current_camera();
    if (current_camera== nullptr){
        return;
    }
    //判断相机的 culling_mask 是否包含当前物体 layer
    if ((current_camera->culling_mask() & game_object()->layer()) == 0x00){
        return;
    }

    glm::mat4 view=current_camera->view_mat4();
    glm::mat4 projection=current_camera->projection_mat4();

    //主动获取 Transform 组件，计算mvp。
    EASY_BLOCK("GetComponent<Transform>()");
    auto component_transform=game_object()->GetComponent<Transform>();
    auto transform=dynamic_cast<Transform*>(component_transform);
    EASY_END_BLOCK;
    if(!transform){
        return;
    }

    EASY_BLOCK("CalculateModelMatrix");
    glm::mat4 trans = glm::translate(transform->position());
    auto rotation=transform->rotation();
    glm::mat4 eulerAngleYXZ = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
    glm::mat4 scale = glm::scale(transform->scale()); //缩放;
    glm::mat4 model = trans*scale*eulerAngleYXZ;
//    glm::mat4 mvp=projection*view * model;
    EASY_END_BLOCK;

    //主动获取 MeshFilter 组件
    EASY_BLOCK("GetComponent<MeshFilter>()");
    auto component_mesh_filter=game_object()->GetComponent<MeshFilter>();
    auto mesh_filter=dynamic_cast<MeshFilter*>(component_mesh_filter);
    EASY_END_BLOCK;
    if(!mesh_filter){
        return;
    }
    //当骨骼蒙皮动画生效时，渲染骨骼蒙皮Mesh
    MeshFilter::Mesh* mesh=mesh_filter->skinned_mesh()== nullptr?mesh_filter->mesh():mesh_filter->skinned_mesh();

    //指定目标Shader程序。
    EASY_BLOCK("GenerateBuffer");
    auto shader=material_->shader();
    GLuint shader_program_handle= shader->shader_program_handle();

    if(vertex_array_object_handle_ == 0){
        vertex_array_object_handle_=GPUResourceMapper::GenerateVAOHandle();
        vertex_buffer_object_handle_=GPUResourceMapper::GenerateVBOHandle();
        //发出任务：创建VAO
        RenderTaskProducer::ProduceRenderTaskCreateVAO(shader_program_handle, vertex_array_object_handle_,
                                                       vertex_buffer_object_handle_,
                                                       mesh->vertex_num_ * sizeof(MeshFilter::Vertex),
                                                       sizeof(MeshFilter::Vertex),
                                                       mesh->vertex_data_,
                                                       mesh->vertex_index_num_ * sizeof(unsigned short),
                                                       mesh->vertex_index_data_);
    }
    else{
        //发出任务：更新VBO
        RenderTaskProducer::ProduceRenderTaskUpdateVBOSubData(vertex_buffer_object_handle_, mesh->vertex_num_ * sizeof(MeshFilter::Vertex),mesh->vertex_data_);
    }
    EASY_END_BLOCK;

    shader->Active();
    {
        // PreRender
        EASY_BLOCK("PreRender");
        game_object()->ForeachComponent([](Component* component){
            component->OnPreRender();
        });
        EASY_END_BLOCK;

        if(current_camera->camera_use_for()==Camera::CameraUseFor::SCENE){
            RenderTaskProducer::ProduceRenderTaskSetEnableState(GL_DEPTH_TEST,true);
        }else{
            RenderTaskProducer::ProduceRenderTaskSetEnableState(GL_DEPTH_TEST,false);
        }
        RenderTaskProducer::ProduceRenderTaskSetEnableState(GL_CULL_FACE,true);
        RenderTaskProducer::ProduceRenderTaskSetEnableState(GL_BLEND,true);
        RenderTaskProducer::ProduceRenderTaskSetBlenderFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //上传mvp矩阵
        RenderTaskProducer::ProduceRenderTaskSetUniformMatrix4fv(shader_program_handle, "u_model", false,model);
        RenderTaskProducer::ProduceRenderTaskSetUniformMatrix4fv(shader_program_handle, "u_view", false,view);
        RenderTaskProducer::ProduceRenderTaskSetUniformMatrix4fv(shader_program_handle, "u_projection", false,projection);

        //上传Texture
        std::vector<std::pair<std::string,Texture2D*>> textures=material_->textures();
        for (int texture_index = 0; texture_index < textures.size(); ++texture_index) {
            //激活纹理单元,将加载的图片纹理句柄，绑定到纹理单元上。
            RenderTaskProducer::ProduceRenderTaskActiveAndBindTexture(GL_TEXTURE0+texture_index,textures[texture_index].second->texture_handle());
            //设置Shader程序从纹理单元读取颜色数据
            RenderTaskProducer::ProduceRenderTaskSetUniform1i(shader_program_handle,textures[texture_index].first.c_str(),texture_index);
        }

        //上传uniform_1f
        std::unordered_map<std::string,float> uniform_1f_map= material_->uniform_1f_map();
        for (auto uniform_1f : uniform_1f_map) {
            RenderTaskProducer::ProduceRenderTaskSetUniform1f(shader_program_handle,uniform_1f.first.c_str(),uniform_1f.second);
        }

        //上传uniform_3f
        std::unordered_map<std::string,glm::vec3> uniform_3f_map= material_->uniform_3f_map();
        for (auto uniform_3f : uniform_3f_map) {
            RenderTaskProducer::ProduceRenderTaskSetUniform3f(shader_program_handle,uniform_3f.first.c_str(),uniform_3f.second);
        }

        // 绑定VAO并绘制
        RenderTaskProducer::ProduceRenderTaskBindVAOAndDrawElements(vertex_array_object_handle_,mesh->vertex_index_num_);

        // PostRender
        EASY_BLOCK("PostRender");
        game_object()->ForeachComponent([](Component* component){
            component->OnPostRender();
        });
        EASY_END_BLOCK;
    }
}

