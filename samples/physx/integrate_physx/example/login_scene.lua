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
require("physics/physics")
require("physics/physic_material")
require("physics/collider")
require("physics/box_collider")
require("physics/sphere_collider")
require("physics/rigid_actor")
require("physics/rigid_dynamic")
require("physics/rigid_static")

LoginScene=class("LoginScene",Component)

--- 登录场景
---@class LoginScene
function LoginScene:ctor()
    LoginScene.super.ctor(self)
    self.go_camera_ = nil
    ---@type Camera
    self.camera_ = nil --相机
    self.go_skeleton_ = nil --骨骼蒙皮动画物体
    self.animation_ = nil--骨骼动画
    self.animation_clip_ = nil --- 骨骼动画片段
    self.material_ = nil --材质
    self.go_ground_ = nil --地面
    self.last_frame_mouse_position_ = nil--上一帧的鼠标位置
end

function LoginScene:Awake()
    print("LoginScene Awake")
    LoginScene.super.Awake(self)
    --创建相机1 GameObject
    self.go_camera_= GameObject.new("main_camera")
    --挂上 Transform 组件
    self.go_camera_:AddComponent(Transform):set_position(glm.vec3(0, 0, 5))
    --挂上 Camera 组件
    self.camera_=self.go_camera_:AddComponent(Camera)
    self.camera_:set_depth(0)

    --创建骨骼蒙皮动画
    self.go_skeleton_=GameObject.new("skeleton")
    self.go_skeleton_:AddComponent(Transform):set_position(glm.vec3(0, 1, 0))
    self.go_skeleton_:GetComponent(Transform):set_rotation(glm.vec3(-90, 0, 0))
    self.go_skeleton_:AddComponent(Animation):LoadAnimationClipFromFile("animation/fbx_extra_bip001_bip001_take_001_baselayer.skeleton_anim","idle")

    local mesh_filter=self.go_skeleton_:AddComponent(MeshFilter)
    mesh_filter:LoadMesh("model/fbx_extra_jiulian.mesh")--加载Mesh
    mesh_filter:LoadWeight("model/fbx_extra_jiulian.weight")--加载权重文件

    --手动创建Material
    self.material_ = Material.new()--设置材质
    self.material_:Parse("material/fbx_extra_jiulian.mat")

    --挂上 MeshRenderer 组件
    local skinned_mesh_renderer= self.go_skeleton_:AddComponent(SkinnedMeshRenderer)
    skinned_mesh_renderer:SetMaterial(self.material_)

    --播放动画
    self.go_skeleton_:GetComponent(Animation):Play("idle")
    self.go_skeleton_:AddComponent(RigidDynamic)
    self.go_skeleton_:AddComponent(SphereCollider)

    --创建地面
    self.go_ground_=GameObject.new("ground")
    self.go_ground_:AddComponent(Transform):set_position(glm.vec3(0, -2, 0))
    self.go_ground_:AddComponent(BoxCollider):set_is_trigger(true)
    self.go_ground_:AddComponent(RigidStatic)
end

function LoginScene:Update()
    --print("LoginScene:Update")
    self.camera_:set_depth(0)
    self.camera_:set_depth(0)
    self.camera_:SetView(glm.vec3(0.0,0.0,0.0), glm.vec3(0.0,1.0,0.0))
    self.camera_:SetPerspective(60, Screen.aspect_ratio(), 1, 1000)

    self.last_frame_mouse_position_=Input.mousePosition()
    --鼠标滚轮控制相机远近
    self.go_camera_:GetComponent(Transform):set_position(self.go_camera_:GetComponent(Transform):position() *(10 - Input.mouse_scroll())/10)
end