LoginScene={
    go_camera_, --相机物体
    go_skeleton_, --骨骼蒙皮动画物体
    animation_,--骨骼动画
    animation_clip_ --- 骨骼动画片段
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
    self.camera_1_=self.go_camera_1_:AddComponent("Camera")
    self.camera_1_:set_depth(0)

    --创建骨骼蒙皮动画
    self.go_skeleton_=GameObject("skeleton")
    self.go_skeleton_:AddComponent("Transform"):set_position(glm.vec3(0, 0, 0))
    self.go_skeleton_:AddComponent("MeshFilter")
    self.go_skeleton_:AddComponent("Animation")

    --手动创建Mesh


    self.animation_clip_=AnimationClip()
    self.animation_clip_:LoadFromFile("animation/export.skeleton_anim")
    self.animation_clip_:Play()
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
    self.animation_clip_:Update()
end