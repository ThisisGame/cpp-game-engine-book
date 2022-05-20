//
// Created by captain on 2021/12/2.
//

#ifndef UNTITLED_SKINNED_MESH_RENDERER_H
#define UNTITLED_SKINNED_MESH_RENDERER_H

#include "mesh_renderer.h"

/// 骨骼蒙皮动画渲染器
class SkinnedMeshRenderer : public MeshRenderer {
public:
    SkinnedMeshRenderer();
    ~SkinnedMeshRenderer();

public:
    // 刷帧，Update在Render之前，在Update里计算最新的蒙皮Mesh
    void Update() override;
};


#endif //UNTITLED_SKINNED_MESH_RENDERER_H
