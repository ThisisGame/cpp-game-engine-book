//
// Created by captain on 2021/12/2.
//

#include "skinned_mesh_renderer.h"
#include <rttr/registration>
#include "component/game_object.h"
#include "mesh_filter.h"
#include "animation.h"
#include "animation_clip.h"
#include "utils/debug.h"

using namespace rttr;
RTTR_REGISTRATION
{
registration::class_<SkinnedMeshRenderer>("SkinnedMeshRenderer")
.constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

SkinnedMeshRenderer::SkinnedMeshRenderer():MeshRenderer() {

}

void SkinnedMeshRenderer::Update() {
    //主动获取 MeshFilter 组件
    auto component_mesh_filter=game_object()->GetComponent("MeshFilter");
    auto mesh_filter=dynamic_cast<MeshFilter*>(component_mesh_filter);
    if(!mesh_filter){
        DEBUG_LOG_ERROR("SkinnedMeshRenderer::Update() failed, can't get MeshFilter component");
        return;
    }
    //获取 Mesh
    auto mesh=mesh_filter->mesh();
    if(!mesh){
        DEBUG_LOG_ERROR("SkinnedMeshRenderer::Update() failed, can't get Mesh");
        return;
    }
    //获取顶点关联骨骼索引数组，长度为顶点个数
    auto vertex_relate_bone_index_vec=mesh_filter->vertex_relate_bone_index_vec();
    if(vertex_relate_bone_index_vec.empty()){
        DEBUG_LOG_ERROR("SkinnedMeshRenderer::Update() failed, can't get vertex relate bone index");
        return;
    }

    //主动获取 Animation 组件
    auto component_animation=game_object()->GetComponent("Animation");
    auto animation=dynamic_cast<Animation*>(component_animation);
    if(!animation){
        DEBUG_LOG_ERROR("SkinnedMeshRenderer::Update() failed, can't get Animation component");
        return;
    }
    //获取当前播放的 AnimationClip
    auto animation_clip=animation->current_animation_clip();
    if(!animation_clip){
        DEBUG_LOG_ERROR("SkinnedMeshRenderer::Update() failed, can't get current AnimationClip");
        return;
    }
    //获取当前帧最新的骨骼矩阵
    std::vector<glm::mat4>& bone_matrices=animation_clip->GetCurrentFrameBoneMatrix();
    //获取 SkinnedMesh
    MeshFilter::Mesh* skinned_mesh=mesh_filter->skinned_mesh();
    if(skinned_mesh==nullptr){
        //拷贝Mesh整体
        skinned_mesh= static_cast<MeshFilter::Mesh *>(malloc(mesh->size()));
        memcpy(skinned_mesh,mesh, mesh->size());
        mesh_filter->set_skinned_mesh(skinned_mesh);

        //拷贝顶点数据 vertex_data_
        skinned_mesh->vertex_data_= static_cast<MeshFilter::Vertex *>(malloc(mesh->vertex_num_*sizeof(MeshFilter::Vertex)));
        memcpy(skinned_mesh->vertex_data_,mesh->vertex_data_, mesh->vertex_num_*sizeof(MeshFilter::Vertex));
    }
    //计算当前帧顶点位置
    for(int i=0;i<skinned_mesh->vertex_num_;i++){
        auto& vertex=mesh->vertex_data_[i];
        auto& bone_index=vertex_relate_bone_index_vec[i];
        auto& bone_matrix=bone_matrices[bone_index];

        glm::vec4 pos_in_model=glm::vec4{vertex.position_,1.0f};
        glm::vec4 pos_in_bone=bone_matrix*pos_in_model;

        skinned_mesh->vertex_data_[i].position_=glm::vec3{pos_in_bone.x,pos_in_bone.y,pos_in_bone.z};
    }
}

SkinnedMeshRenderer::~SkinnedMeshRenderer() {

}