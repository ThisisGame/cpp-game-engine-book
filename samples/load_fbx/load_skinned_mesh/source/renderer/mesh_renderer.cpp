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
#include "material.h"
#include "mesh_filter.h"
#include "texture2d.h"
#include "shader.h"
#include "camera.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "utils/debug.h"

using timetool::StopWatch;
using namespace rttr;
RTTR_REGISTRATION
{
    registration::class_<MeshRenderer>("MeshRenderer")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

MeshRenderer::MeshRenderer():Component() {

}

MeshRenderer::~MeshRenderer() {

}

void MeshRenderer::SetMaterial(Material* material) {
    material_=material;
}


void MeshRenderer::Render() {
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
    auto component_transform=game_object()->GetComponent("Transform");
    auto transform=dynamic_cast<Transform*>(component_transform);
    if(!transform){
        return;
    }

    glm::mat4 trans = glm::translate(transform->position());
    auto rotation=transform->rotation();
    glm::mat4 eulerAngleYXZ = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
    glm::mat4 scale = glm::scale(transform->scale()); //缩放;
    glm::mat4 model = trans*scale*eulerAngleYXZ;
    glm::mat4 mvp=projection*view * model;

    //主动获取 MeshFilter 组件
    auto component_mesh_filter=game_object()->GetComponent("MeshFilter");
    auto mesh_filter=dynamic_cast<MeshFilter*>(component_mesh_filter);
    if(!mesh_filter){
        return;
    }
    //当骨骼蒙皮动画生效时，渲染骨骼蒙皮Mesh
    MeshFilter::Mesh* mesh=mesh_filter->skinned_mesh()== nullptr?mesh_filter->mesh():mesh_filter->skinned_mesh();

    //获取`Shader`的`gl_program_id`，指定为目标Shader程序。
    GLuint gl_program_id=material_->shader()->gl_program_id();

    if(vertex_array_object_ == 0){
        GLint vpos_location = glGetAttribLocation(gl_program_id, "a_pos");
        GLint vcol_location = glGetAttribLocation(gl_program_id, "a_color");
        GLint a_uv_location = glGetAttribLocation(gl_program_id, "a_uv");

        //在GPU上创建缓冲区对象
        glGenBuffers(1,&vertex_buffer_object_);
        //将缓冲区对象指定为顶点缓冲区对象
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
        //上传顶点数据到缓冲区对象
        glBufferData(GL_ARRAY_BUFFER, mesh->vertex_num_ * sizeof(MeshFilter::Vertex), mesh->vertex_data_, GL_DYNAMIC_DRAW);

        //在GPU上创建缓冲区对象
        glGenBuffers(1,&element_buffer_object_);
        //将缓冲区对象指定为顶点索引缓冲区对象
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
        //上传顶点索引数据到缓冲区对象
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->vertex_index_num_ * sizeof(unsigned short), mesh->vertex_index_data_, GL_STATIC_DRAW);

        glGenVertexArrays(1,&vertex_array_object_);

        //设置VAO
        glBindVertexArray(vertex_array_object_);__CHECK_GL_ERROR__
        {
            //指定当前使用的VBO
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);__CHECK_GL_ERROR__
            //将Shader变量(a_pos)和顶点坐标VBO句柄进行关联，最后的0表示数据偏移量。
            glVertexAttribPointer(vpos_location, 3, GL_FLOAT, false, sizeof(MeshFilter::Vertex), 0);__CHECK_GL_ERROR__
            //启用顶点Shader属性(a_color)，指定与顶点颜色数据进行关联
            glVertexAttribPointer(vcol_location, 4, GL_FLOAT, false, sizeof(MeshFilter::Vertex), (void*)(sizeof(float)*3));__CHECK_GL_ERROR__
            //将Shader变量(a_uv)和顶点UV坐标VBO句柄进行关联，最后的0表示数据偏移量。
            glVertexAttribPointer(a_uv_location, 2, GL_FLOAT, false, sizeof(MeshFilter::Vertex), (void*)(sizeof(float)*(3+4)));__CHECK_GL_ERROR__

            glEnableVertexAttribArray(vpos_location);__CHECK_GL_ERROR__
            glEnableVertexAttribArray(vcol_location);__CHECK_GL_ERROR__
            glEnableVertexAttribArray(a_uv_location);__CHECK_GL_ERROR__

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);__CHECK_GL_ERROR__
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);__CHECK_GL_ERROR__
    }
    else{
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);__CHECK_GL_ERROR__
        StopWatch stopwatch;
        stopwatch.start();
        //更新Buffer数据
        glBufferSubData(GL_ARRAY_BUFFER,0,mesh->vertex_num_ * sizeof(MeshFilter::Vertex),mesh->vertex_data_);__CHECK_GL_ERROR__
        stopwatch.stop();
        DEBUG_LOG_INFO("glBufferSubData cost {}",stopwatch.microseconds());
    }

    glUseProgram(gl_program_id);
    {
        // PreRender
        game_object()->ForeachLuaComponent([](sol::table lua_component_instance_table){
            sol::protected_function function=lua_component_instance_table["OnPreRender"];
            if(function.valid()){
                auto result=function(lua_component_instance_table);
                if(result.valid()== false){
                    sol::error err = result;
                    DEBUG_LOG_ERROR("\n---- RUN LUA ERROR ----\n{}\n------------------------",err.what());
                }
            }
        });

        if(current_camera->camera_use_for()==Camera::CameraUseFor::SCENE){
            glEnable(GL_DEPTH_TEST);__CHECK_GL_ERROR__
        }else{
            glDisable(GL_DEPTH_TEST);__CHECK_GL_ERROR__
        }
        glEnable(GL_CULL_FACE);__CHECK_GL_ERROR__//开启背面剔除
        glEnable(GL_BLEND);__CHECK_GL_ERROR__
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);__CHECK_GL_ERROR__

        //上传mvp矩阵
        GLint uniform_location=glGetUniformLocation(gl_program_id, "u_mvp");
        glUniformMatrix4fv(uniform_location, 1, GL_FALSE, &mvp[0][0]);__CHECK_GL_ERROR__

        //从Pass节点拿到保存的Texture
        std::vector<std::pair<std::string,Texture2D*>> textures=material_->textures();
        for (int texture_index = 0; texture_index < textures.size(); ++texture_index) {
            GLint u_texture_location= glGetUniformLocation(gl_program_id, textures[texture_index].first.c_str());
            //激活纹理单元0
            glActiveTexture(GL_TEXTURE0+texture_index);
            //将加载的图片纹理句柄，绑定到纹理单元0的Texture2D上。
            glBindTexture(GL_TEXTURE_2D,textures[texture_index].second->gl_texture_id());
            //设置Shader程序从纹理单元0读取颜色数据
            glUniform1i(u_texture_location,texture_index);
        }

        glBindVertexArray(vertex_array_object_);
        {
            StopWatch stopwatch;
            stopwatch.start();
            glDrawElements(GL_TRIANGLES,mesh->vertex_index_num_,GL_UNSIGNED_SHORT,0);//使用顶点索引进行绘制，最后的0表示数据偏移量。
            stopwatch.stop();
            DEBUG_LOG_INFO("glDrawElements cost {}",stopwatch.microseconds());
        }
        glBindVertexArray(0);__CHECK_GL_ERROR__

        // PostRender
        game_object()->ForeachLuaComponent([](sol::table lua_component_instance_table){
            sol::protected_function function=lua_component_instance_table["OnPostRender"];
            if(function.valid()){
                auto result=function(lua_component_instance_table);
                if(result.valid()== false){
                    sol::error err = result;
                    DEBUG_LOG_ERROR("\n---- RUN LUA ERROR ----\n{}\n------------------------",err.what());
                }
            }
        });
    }
}

