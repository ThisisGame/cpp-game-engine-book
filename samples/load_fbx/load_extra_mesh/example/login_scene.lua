require("lua_extension")
require("renderer/camera")
require("renderer/mesh_filter")
require("renderer/mesh_renderer")
require("renderer/material")
require("renderer/animation")
require("renderer/animation_clip")
require("renderer/skinned_mesh_renderer")
require("control/input")
require("control/key_code")
require("audio/studio/audio_studio")
require("utils/screen")
require("utils/time")

LoginScene=class("LoginScene",Component)

--- 登录场景
---@class LoginScene
function LoginScene:ctor()
    LoginScene.super.ctor(self)
    self.go_camera_ = nil
    ---@type Camera
    self.camera_ = nil --相机
    self.go_mesh_ = nil --骨骼蒙皮动画物体
    self.material_ = nil --材质
end

function LoginScene:Awake()
    print("LoginScene Awake")
    LoginScene.super.Awake(self)
    --创建相机1 GameObject
    self.go_camera_= GameObject.new("main_camera")
    --挂上 Transform 组件
    self.go_camera_:AddComponent(Transform):set_position(glm.vec3(0, 0, 10))
    --挂上 Camera 组件
    self.camera_=self.go_camera_:AddComponent(Camera)
    self.camera_:set_depth(0)

    --创建骨骼蒙皮动画
    self.go_mesh_=GameObject.new("mesh")
    self.go_mesh_:AddComponent(Transform):set_position(glm.vec3(0, 0, 0))
    self.go_mesh_:GetComponent(Transform):set_rotation(glm.vec3(-90, 0, 0))

    --- @type MeshFilter
    local mesh_filter=self.go_mesh_:AddComponent(MeshFilter)
    mesh_filter:LoadMesh("model/fbx_extra_jiulian.mesh")--加载Mesh
    mesh_filter:game_object():set_name(mesh_filter:GetMeshName())

    --手动创建Material
    self.material_ = Material.new()--设置材质
    self.material_:Parse("material/fbx_extra_jiulian.mat")

    --挂上 MeshRenderer 组件
    local mesh_renderer= self.go_mesh_:AddComponent(MeshRenderer)
    mesh_renderer:SetMaterial(self.material_)
end

function LoginScene:Update()
    --print("LoginScene:Update")
    self.camera_:set_depth(0)
    --使用正交相机
    local aspect_ratio=Screen.aspect_ratio()
    --相机尺寸Width设置为正负5
    local OrthographicWidth=5
    local OrthographicHeight=OrthographicWidth/aspect_ratio
    self.camera_:SetOrthographic(-OrthographicWidth,OrthographicWidth,-OrthographicHeight,OrthographicHeight,-100,100)


    local camera_transform=self.go_camera_:GetComponent(Transform)
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