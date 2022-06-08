require("lua_extension")
require("lua_extension_math")
require("renderer/camera")
require("renderer/mesh_filter")
require("renderer/mesh_renderer")
require("renderer/material")
require("renderer/animation")
require("renderer/animation_clip")
require("renderer/skinned_mesh_renderer")
require("renderer/texture_2d")
require("control/input")
require("control/key_code")
require("utils/screen")
require("utils/time")
require("lighting/environment")

LoginScene=class("LoginScene",Component)

--- 登录场景
---@class LoginScene
function LoginScene:ctor()
    LoginScene.super.ctor(self)
    self.go_camera_ = nil
    ---@field camera_ Camera @场景相机
    self.camera_ = nil
    self.go_skeleton_ = nil --骨骼蒙皮动画物体
    self.animation_ = nil--骨骼动画
    self.animation_clip_ = nil --- 骨骼动画片段
    self.material_ = nil --材质
    self.environment_=nil --环境
end

function LoginScene:Awake()
    print("LoginScene Awake")
    LoginScene.super.Awake(self)

    self:CreateEnvironment()
    self:CreateMainCamera()
    self:CreatePlayer()
end

function LoginScene:CreateEnvironment()
    self.environment_=Environment.new()
    self.environment_:set_ambient_color(glm.vec3(1.0,1.0,1.0))
    self.environment_:set_ambient_color_intensity(0.7)
end

--- 创建主相机
function LoginScene:CreateMainCamera()
    --创建相机1 GameObject
    self.go_camera_= GameObject.new("main_camera")
    --挂上 Transform 组件
    self.go_camera_:AddComponent(Transform):set_position(glm.vec3(0, 10, 20))
    self.go_camera_:GetComponent(Transform):set_rotation(glm.vec3(-10, 0, 0))
    --挂上 Camera 组件
    self.camera_=self.go_camera_:AddComponent(Camera)
    --设置为黑色背景
    self.camera_:set_clear_color(0,0,0,1)
end

--- 创建Player
function LoginScene:CreatePlayer()
    --创建骨骼蒙皮动画
    self.go_skeleton_=GameObject.new("skeleton")
    self.go_skeleton_:AddComponent(Transform):set_position(glm.vec3(0, 4, 0))
    self.go_skeleton_:GetComponent(Transform):set_rotation(glm.vec3(0, 0, 0))
    local anim_clip_name="animation/fbx_extra_basic_plane_bones_basic_plane_bones_basic_plane_bones_armatureaction_basic_plane_.skeleton_anim"
    self.go_skeleton_:AddComponent(Animation):LoadAnimationClipFromFile(anim_clip_name,"idle")

    local mesh_filter=self.go_skeleton_:AddComponent(MeshFilter)
    mesh_filter:LoadMesh("model/basic_plane_model_basic_plane.mesh")--加载Mesh
    mesh_filter:LoadWeight("model/fbx_extra_basic_plane.weight")--加载权重文件

    --手动创建Material
    self.material_ = Material.new()--设置材质
    self.material_:Parse("material/basic_plane_diffuse_light.mat")

    --挂上 MeshRenderer 组件
    local skinned_mesh_renderer= self.go_skeleton_:AddComponent(SkinnedMeshRenderer)
    skinned_mesh_renderer:SetMaterial(self.material_)

    --播放动画
    self.go_skeleton_:GetComponent(Animation):Play("idle")
end

function LoginScene:Update()
    --print("LoginScene:Update")
    LoginScene.super.Update(self)
    self.camera_:set_depth(0)
    self.camera_:SetView(glm.vec3(0.0,0.0,0.0), glm.vec3(0.0,1.0,0.0))
    self.camera_:SetPerspective(60, Screen.aspect_ratio(), 1, 1000)

    --按键R G B切换环境光颜色
    if Input.GetKeyUp(KeyCode.KEY_CODE_R) then
        self.environment_:set_ambient_color(glm.vec3(1.0,0.0,0.0))
    elseif Input.GetKeyUp(KeyCode.KEY_CODE_G) then
        self.environment_:set_ambient_color(glm.vec3(0.0,1.0,0.0))
    elseif Input.GetKeyUp(KeyCode.KEY_CODE_B) then
        self.environment_:set_ambient_color(glm.vec3(0.0,0.0,1.0))
    end

    --方向上下切换环境光强度
    local ambient_light_intensity=self.environment_:ambient_color_intensity()
    if Input.GetKeyDown(KeyCode.KEY_CODE_UP) then
        ambient_light_intensity=ambient_light_intensity+Time:delta_time()*0.5
    elseif Input.GetKeyDown(KeyCode.KEY_CODE_DOWN) then
        ambient_light_intensity=ambient_light_intensity-Time:delta_time()*0.5
    end
    self.environment_:set_ambient_color_intensity(math.clamp(ambient_light_intensity,0,1))

    --设置环境光颜色和强度
    self.material_:SetUniform3f("u_ambient_light_color",self.environment_:ambient_color())
    self.material_:SetUniform1f("u_ambient_light_intensity",self.environment_:ambient_color_intensity())

    --鼠标滚轮控制相机远近
    self.go_camera_:GetComponent(Transform):set_position(self.go_camera_:GetComponent(Transform):position() *(10 - Input.mouse_scroll())/10)
end