LoginScene={
    go_camera_, --相机物体
    camera_ = nil, --相机
    go_skeleton_, --骨骼蒙皮动画物体
    material_, --材质
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
    self.go_skeleton_=GameObject("mesh")
    self.go_skeleton_:AddComponent("Transform"):set_position(glm.vec3(0, 0, 0))
    --self.go_skeleton_:GetComponent("Transform"):set_rotation(glm.vec3(-90, 0, 0))
    self.go_skeleton_:AddComponent("Animation"):LoadAnimationClipFromFile("animation/assimp_extra_take_001.skeleton_anim","idle")

    local mesh_filter=self.go_skeleton_:AddComponent("MeshFilter")
    mesh_filter:LoadMesh("model/assimp_extra_jiulian.mesh")--加载Mesh
    mesh_filter:game_object():set_name(mesh_filter:GetMeshName())
    mesh_filter:LoadWeight("model/assimp_extra_jiulian.weight")--加载权重文件

    --手动创建Material
    self.material_ = Material()--设置材质
    self.material_:Parse("material/assimp_extra_jiulian.mat")

    --挂上 MeshRenderer 组件
    local mesh_renderer= self.go_skeleton_:AddComponent("SkinnedMeshRenderer")
    mesh_renderer:SetMaterial(self.material_)
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
    --self.camera_:SetPerspective(60, Screen.aspect_ratio(), 1, 1000)
    --使用正交相机
    local aspect_ratio=Screen.aspect_ratio()
    --相机尺寸Width设置为正负5
    local OrthographicWidth=5
    local OrthographicHeight=OrthographicWidth/aspect_ratio
    self.camera_:SetOrthographic(-OrthographicWidth,OrthographicWidth,-OrthographicHeight,OrthographicHeight,-100,100)


    local camera_transform=self.go_camera_:GetComponent("Transform")
    local camera_position=camera_transform:position()


    --WASD控制相机上下左右移动
    if Input.GetKeyDown(KeyCode.KEY_CODE_W) then
        camera_transform:set_position(camera_position+glm.vec3(0,0.1,0))
    elseif Input.GetKeyDown(KeyCode.KEY_CODE_S) then
        camera_transform:set_position(camera_position+glm.vec3(0,-0.1,0))
    elseif Input.GetKeyDown(KeyCode.KEY_CODE_A) then
        camera_transform:set_position(camera_position+glm.vec3(-0.1,0,0))
    elseif Input.GetKeyDown(KeyCode.KEY_CODE_D) then
        camera_transform:set_position(camera_position+glm.vec3(0.1,0,0))
    end
    self.camera_:SetView(glm.vec3(camera_position.x,camera_position.y,0.0), glm.vec3(0.0,1.0,0.0))
end