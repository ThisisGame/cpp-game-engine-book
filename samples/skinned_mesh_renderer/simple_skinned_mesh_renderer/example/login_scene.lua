LoginScene={
    go_camera_, --相机物体
    camera_ = nil, --相机
    go_skeleton_, --骨骼蒙皮动画物体
    animation_,--骨骼动画
    animation_clip_, --- 骨骼动画片段
    material_, --材质
    last_frame_mouse_position_,--上一帧的鼠标位置
}

setmetatable(LoginScene,{["__call"]=function(table,param)
    local instance=setmetatable({},{__index=table})
    return instance
end})

-- public:
function LoginScene:Awake()
    --创建相机1 GameObject
    self.go_camera_= GameObject("main_camera")
    --挂上 Transform 组件
    self.go_camera_:AddComponent("Transform"):set_position(glm.vec3(0, 0, 10))
    --挂上 Camera 组件
    self.camera_=self.go_camera_:AddComponent("Camera")
    self.camera_:set_depth(0)

    --创建骨骼蒙皮动画
    self.go_skeleton_=GameObject("skeleton")
    self.go_skeleton_:AddComponent("Transform"):set_position(glm.vec3(0, 0, 0))
    self.go_skeleton_:AddComponent("Animation"):LoadAnimationClipFromFile("animation/export.skeleton_anim","idle")

    --手动创建Mesh
    local vertex_data=sol2.convert_sequence_float({
        -0.2,0,0,  1.0,1.0,1.0,1.0, 0,0,--与 Bone 关联的顶点，就是下面那一根骨骼。
         0.2,0,0,  1.0,1.0,1.0,1.0, 1,0,
         0.2,2,0,  1.0,1.0,1.0,1.0, 1,1,
        -0.2,2,0,  1.0,1.0,1.0,1.0, 0,1,

        -0.2,2,0,  1.0,1.0,1.0,1.0, 0,0,--与 Bone.001 关联的顶点，就是上面一根骨骼。
         0.2,2,0,  1.0,1.0,1.0,1.0, 1,0,
         0.2,3,0,  1.0,1.0,1.0,1.0, 1,1,
        -0.2,3,0,  1.0,1.0,1.0,1.0, 0,1,
    })
    local vertex_index_data=sol2.convert_sequence_ushort({
        0,1,2,
        0,2,3,

        4,5,6,
        4,6,7
    })
    local vertex_relate_bone_index_vec=sol2.convert_sequence_uchar({--顶点关联的骨骼序号
        0, 0, 0, 0,
        1, 1, 1, 1
    })
    local mesh_filter=self.go_skeleton_:AddComponent("MeshFilter")
    mesh_filter:CreateMesh(vertex_data,vertex_index_data)--手动构建Mesh
    mesh_filter:set_vertex_relate_bone_index_vec(vertex_relate_bone_index_vec)

    --手动创建Material
    self.material_ = Material()--设置材质
    self.material_:Parse("material/cube.mat")

    --挂上 MeshRenderer 组件
    local skinned_mesh_renderer= self.go_skeleton_:AddComponent("SkinnedMeshRenderer")
    skinned_mesh_renderer:SetMaterial(self.material_)

    --播放动画
    self.go_skeleton_:GetComponent("Animation"):Play("idle")
end

function LoginScene:game_object()
    print("LoginScene:game_object")
    return self.game_object_
end

function LoginScene:set_game_object(game_object)
    print("LoginScene:set_game_object " .. tostring(game_object) .. " self:" .. tostring(self))
    self.game_object_=game_object
end

function LoginScene:Update()
    --print("LoginScene:Update")
    self.camera_:set_depth(0)
    self.camera_:SetView(glm.vec3(0.0,0.0,0.0), glm.vec3(0.0,1.0,0.0))
    self.camera_:SetPerspective(60, Screen.aspect_ratio(), 1, 1000)

    self.last_frame_mouse_position_=Input.mousePosition()
    --鼠标滚轮控制相机远近
    self.go_camera_:GetComponent("Transform"):set_position(self.go_camera_:GetComponent("Transform"):position() *(10 - Input.mouse_scroll())/10)
end