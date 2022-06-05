require("lua_extension")
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
require("audio/studio/audio_studio")
require("utils/screen")
require("utils/time")
require("physics/physics")
require("physics/physic_material")
require("physics/collider")
require("physics/box_collider")
require("physics/sphere_collider")
require("physics/rigid_actor")
require("physics/rigid_dynamic")
require("physics/rigid_static")
require("ui/ui_camera")
require("ui/ui_image")
require("player")

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
    self.go_ground_ = nil --地面
    self.last_frame_mouse_position_ = nil--上一帧的鼠标位置
    ---@field go_ui_camera_ GameObject @UI相机物体
    self.go_ui_camera_=nil
    ---@field go_image_warning_ GameObject @坠机警告图片物体
    self.go_image_warning_=nil
end

function LoginScene:Awake()
    print("LoginScene Awake")
    LoginScene.super.Awake(self)

    self:CreateMainCamera()
    self:CreatePlayer()
    self:CreateGround()
    self:CreateUI()
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
    self.camera_:set_depth(0)
end

--- 创建Player
function LoginScene:CreatePlayer()
    --创建骨骼蒙皮动画
    self.go_skeleton_=GameObject.new("skeleton")
    self.go_skeleton_:AddComponent(Transform):set_position(glm.vec3(0, 20, 0))
    self.go_skeleton_:GetComponent(Transform):set_rotation(glm.vec3(0, 0, 0))
    local anim_clip_name="animation/fbx_extra_basic_plane_bones_basic_plane_bones_basic_plane_bones_armatureaction_basic_plane_.skeleton_anim"
    self.go_skeleton_:AddComponent(Animation):LoadAnimationClipFromFile(anim_clip_name,"idle")

    local mesh_filter=self.go_skeleton_:AddComponent(MeshFilter)
    mesh_filter:LoadMesh("model/fbx_extra_basic_plane.mesh")--加载Mesh
    mesh_filter:LoadWeight("model/fbx_extra_basic_plane.weight")--加载权重文件

    --手动创建Material
    self.material_ = Material.new()--设置材质
    self.material_:Parse("material/fbx_extra_basic_plane.mat")

    --挂上 MeshRenderer 组件
    local skinned_mesh_renderer= self.go_skeleton_:AddComponent(SkinnedMeshRenderer)
    skinned_mesh_renderer:SetMaterial(self.material_)

    --播放动画
    self.go_skeleton_:GetComponent(Animation):Play("idle")
    self.go_skeleton_:AddComponent(RigidDynamic)
    self.go_skeleton_:AddComponent(SphereCollider):set_is_trigger(false)
    self.go_skeleton_:AddComponent(Player)
end

--- 创建地面
function LoginScene:CreateGround()
    --创建地面
    self.go_ground_=GameObject.new("ground")
    self.go_ground_:AddComponent(Transform):set_position(glm.vec3(0, -2, 0))
    self.go_ground_:AddComponent(RigidStatic)
    self.go_ground_:AddComponent(BoxCollider):set_is_trigger(false)
end

--- 创建UI
function LoginScene:CreateUI()
    --创建UI相机 GameObject
    self.go_ui_camera_= GameObject.new("ui_camera")
    --挂上 Transform 组件
    local trans=self.go_ui_camera_:AddComponent(Transform)
    trans:set_position(glm.vec3(0, 10, 20))
    trans:set_rotation(glm.vec3(-10, 0, 0))
    --挂上 Camera 组件
    local ui_camera_=self.go_ui_camera_:AddComponent(UICamera)
    ui_camera_:set_depth(1)
    ui_camera_:set_culling_mask(0x02);
    --UI相机不能清除之前的颜色。不然用第一个相机矩阵渲染的物体就被清除 没了。
    ui_camera_:set_clear_flag(Cpp.BufferClearFlag.CLEAR_DEPTH_BUFFER);
    --设置正交相机
    ui_camera_:SetView(glm.vec3(0, 0, 0), glm.vec3(0, 1, 0));
    ui_camera_:SetOrthographic(-Screen:width()/2,Screen:width()/2,-Screen:height()/2,Screen:height()/2,-100,100);

    --创建 GameObject
    self.go_image_warning_=GameObject.new("image_warning");
    self.go_image_warning_:set_layer(0x02);
    ----挂上 Transform 组件
    self.go_image_warning_:AddComponent(Transform);
    ----挂上 UIImage 组件
    local image_warning=self.go_image_warning_:AddComponent(UIImage)
    local texture_warning=Texture2D.LoadFromFile("images/warning.cpt")
    image_warning:set_texture(texture_warning);
    self.go_image_warning_:GetComponent(Transform):set_position(glm.vec3(-texture_warning:width()/2,0,0))
end

function LoginScene:Update()
    --print("LoginScene:Update")
    LoginScene.super.Update(self)
    self.camera_:set_depth(0)
    self.camera_:SetView(glm.vec3(0.0,0.0,0.0), glm.vec3(0.0,1.0,0.0))
    self.camera_:SetPerspective(60, Screen.aspect_ratio(), 1, 1000)

    --警告图片闪烁
    local show=(math.floor(Time:TimeSinceStartup()*10)%3)==0
    self.go_image_warning_:set_active(show)

    self.last_frame_mouse_position_=Input.mousePosition()
    --鼠标滚轮控制相机远近
    self.go_camera_:GetComponent(Transform):set_position(self.go_camera_:GetComponent(Transform):position() *(10 - Input.mouse_scroll())/10)
end