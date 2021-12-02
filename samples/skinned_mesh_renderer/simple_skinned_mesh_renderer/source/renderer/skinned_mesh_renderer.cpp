//
// Created by captain on 2021/12/2.
//

#include "skinned_mesh_renderer.h"
#include "mesh_filter.h"
#include "animation.h"
#include "utils/debug.h"

SkinnedMeshRenderer::SkinnedMeshRenderer() {

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
    std::vector<glm::mat4>& bone_matrices=animation_clip->GetCurrentFrameBoneMatrix()

}

SkinnedMeshRenderer::~SkinnedMeshRenderer() {

}