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
require("renderer/render_texture")
require("control/input")
require("control/key_code")
require("utils/screen")
require("utils/time")
require("lighting/environment")
require("lighting/point_light")
require("lighting/directional_light")

LoginScene=class("LoginScene",Component)

--- 登录场景
---@class LoginScene
function LoginScene:ctor()
    LoginScene.super.ctor(self)

    self.go_depth_camera_ = nil
    ---@field depth_camera_ Camera @深度相机
    self.depth_camera_ = nil
    ---@field depth_render_texture_ RenderTexture  @深度RTT
    self.depth_render_texture_ = nil

    self.go_camera_ = nil
    ---@field camera_ Camera @场景相机
    self.camera_ = nil

    self.go_skeleton_ = nil --骨骼蒙皮动画物体
    self.animation_ = nil--骨骼动画
    self.animation_clip_ = nil --- 骨骼动画片段
    self.material_plane_ = nil --材质
    self.environment_=nil --环境
    self.go_light_=nil --灯光父节点
    self.go_point_light_1_=nil --灯光
    self.go_point_light_2_=nil --灯光
    self.go_wall_=nil--墙壁
    self.material_wall_=nil
    self.last_frame_mouse_position_=nil--上一帧的鼠标位置
end

function LoginScene:Awake()
    print("LoginScene Awake")
    LoginScene.super.Awake(self)

    self:CreateEnvironment()
    self:CreateLight()
    self:CreateDepthCamera()
    self:CreateMainCamera()
    self:CreatePlane()
    self:CreateWall()
end

--- 创建环境
function LoginScene:CreateEnvironment()
    self.environment_=Environment.new()
    self.environment_:set_ambient_color(glm.vec3(1.0,1.0,1.0))
    self.environment_:set_ambient_color_intensity(0.3)
end

function LoginScene:CreateLight()
    self.go_light_=GameObject.new("light")
    self.go_light_:AddComponent(Transform)

    self:CreateDirectionalLight1()
end

--- 创建方向光1
function LoginScene:CreateDirectionalLight1()
    self.go_directional_light_1_= GameObject.new("directional_light_1")
    self.go_directional_light_1_:AddComponent(Transform)
    self.go_directional_light_1_:GetComponent(Transform):set_rotation(glm.vec3(0,0,20))

    local light=self.go_directional_light_1_:AddComponent(DirectionalLight)
    light:set_color(glm.vec3(1.0,1.0,1.0))
    light:set_intensity(1.0)

    self.go_light_:AddChild(self.go_directional_light_1_)
end

--- 创建深度相机
function LoginScene:CreateDepthCamera()
    --创建相机1 GameObject
    self.go_depth_camera_= GameObject.new("depth_camera")
    --挂上 Transform 组件
    self.go_depth_camera_:AddComponent(Transform):set_position(glm.vec3(0, 0, 10))
    self.go_depth_camera_:GetComponent(Transform):set_rotation(glm.vec3(0, 0, 0))
    --挂上 Camera 组件
    self.depth_camera_=self.go_depth_camera_:AddComponent(Camera)
    --设置为黑色背景
    self.depth_camera_:set_clear_color(0,0,0,1)
    self.depth_camera_:set_depth(0)
    self.depth_camera_:SetView(glm.vec3(0.0,0.0,0.0), glm.vec3(0.0,1.0,0.0))
    self.depth_camera_:SetPerspective(60, Screen.aspect_ratio(), 1, 1000)
    --设置RenderTexture
    self.depth_render_texture_ = RenderTexture.new()
    self.depth_render_texture_:Init(480,320)
    self.depth_camera_:set_target_render_texture(self.depth_render_texture_)
end

--- 创建主相机
function LoginScene:CreateMainCamera()
    --创建相机1 GameObject
    self.go_camera_= GameObject.new("main_camera")
    --挂上 Transform 组件
    self.go_camera_:AddComponent(Transform):set_position(glm.vec3(0, 0, 10))
    self.go_camera_:GetComponent(Transform):set_rotation(glm.vec3(0, 0, 0))
    --挂上 Camera 组件
    self.camera_=self.go_camera_:AddComponent(Camera)
    --设置为黑色背景
    self.camera_:set_clear_color(0,0,0,1)
    self.camera_:set_depth(0)
    self.camera_:SetView(glm.vec3(0.0,0.0,0.0), glm.vec3(0.0,1.0,0.0))
    self.camera_:SetPerspective(60, Screen.aspect_ratio(), 1, 1000)
end

--- 创建模型
function LoginScene:CreatePlane()
    --创建骨骼蒙皮动画
    self.go_skeleton_=GameObject.new("skeleton")
    self.go_skeleton_:AddComponent(Transform):set_position(glm.vec3(0, 0, 0))
    self.go_skeleton_:GetComponent(Transform):set_rotation(glm.vec3(0, 0, 0))
    local anim_clip_name="animation/fbx_extra_basic_plane_bones_basic_plane_bones_basic_plane_bones_armatureaction_basic_plane_.skeleton_anim"
    self.go_skeleton_:AddComponent(Animation):LoadAnimationClipFromFile(anim_clip_name,"idle")

    local mesh_filter=self.go_skeleton_:AddComponent(MeshFilter)
    mesh_filter:LoadMesh("model/basic_plane_model_basic_plane.mesh")--加载Mesh
    mesh_filter:LoadWeight("model/fbx_extra_basic_plane.weight")--加载权重文件

    --手动创建Material
    self.material_plane_ = Material.new()--设置材质
    self.material_plane_:Parse("material/basic_plane_multi_light.mat")

    --挂上 MeshRenderer 组件
    local skinned_mesh_renderer= self.go_skeleton_:AddComponent(SkinnedMeshRenderer)
    skinned_mesh_renderer:SetMaterial(self.material_plane_)

    --播放动画
    self.go_skeleton_:GetComponent(Animation):Play("idle")
end

---手动创建Mesh
---@return void
function LoginScene:CreateWall()
    local vertex_data={
        -10,-10,0,  1.0,1.0,1.0,1.0, 0,0, -10,-10,1,
         10,-10,0,  1.0,1.0,1.0,1.0, 1,0, 10,-10,1,
         10, 10,0,  1.0,1.0,1.0,1.0, 1,1, 10, 10,1,
        -10, 10,0,  1.0,1.0,1.0,1.0, 0,1, -10, 10,1,
    }
    local vertex_index_data={
        0,1,2,
        0,2,3,
    }

    self.go_wall_=GameObject.new("wall")
    self.go_wall_:AddComponent(Transform):set_position(glm.vec3(0, 0, -10))
    self.go_wall_:GetComponent(Transform):set_rotation(glm.vec3(0, 0, 0))

    local mesh_filter=self.go_wall_:AddComponent(MeshFilter)
    mesh_filter:CreateMesh(sol2.convert_sequence_float(vertex_data),sol2.convert_sequence_ushort(vertex_index_data))--手动构建Mesh

    --手动创建Material
    self.material_wall_ = Material.new()--设置材质
    self.material_wall_:Parse("material/wall.mat")

    --挂上 MeshRenderer 组件
    local mesh_renderer= self.go_wall_:AddComponent(MeshRenderer)
    mesh_renderer:SetMaterial(self.material_wall_)
end

function LoginScene:Update()
    --print("LoginScene:Update")
    LoginScene.super.Update(self)

    --设置观察者世界坐标(即相机位置)
    local camera_position=self.go_camera_:GetComponent(Transform):position()
    self.material_plane_:SetUniform3f("u_view_pos",camera_position)
    --设置物体反射度、高光强度
    self.material_plane_:SetUniform1f("u_specular_highlight_shininess",32.0)

    --鼠标滚轮控制相机远近
    self.go_camera_:GetComponent(Transform):set_position(self.go_camera_:GetComponent(Transform):position() *(10 - Input.mouse_scroll())/10)

    --旋转相机
    if Input.GetKeyDown(Cpp.KeyCode.KEY_CODE_LEFT_ALT) and Input.GetMouseButtonDown(Cpp.KeyCode.MOUSE_BUTTON_LEFT) and self.last_frame_mouse_position_ then
        --print(Input.mousePosition(),self.last_frame_mouse_position_)
        local degrees= Input.mousePosition().x - self.last_frame_mouse_position_.x
        self.last_frame_mouse_position_=Input.mousePosition()

        local old_mat4=glm.mat4(1.0)
        local rotate_mat4=glm.rotate(old_mat4,glm.radians(degrees),glm.vec3(0.0,1.0,0.0))--以相机所在坐标系位置，计算用于旋转的矩阵，这里是零点，所以直接用方阵。

        local camera_pos=self.go_camera_:GetComponent(Transform):position()
        local old_pos=glm.vec4(camera_pos.x,camera_pos.y,camera_pos.z,1.0)
        local new_pos=rotate_mat4*old_pos--旋转矩阵 * 原来的坐标 = 相机以零点做旋转。
        --print(new_pos)
        self.go_camera_:GetComponent(Transform):set_position(glm.vec3(new_pos.x,new_pos.y,new_pos.z))
    end
    self.last_frame_mouse_position_=Input.mousePosition()
end