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
require("renderer/render_texture_geometry_buffer")
require("renderer/noise_texture")
require("control/input")
require("control/key_code")
require("utils/screen")
require("utils/time")
require("utils/object_reference_manager")
require("lighting/environment")
require("lighting/point_light")
require("lighting/directional_light")
require("ui/rect_transform")
require("ui/ui_camera")
require("ui/ui_image")

LoginScene=class("LoginScene",Component)

--- 登录场景
---@class LoginScene
function LoginScene:ctor()
    LoginScene.super.ctor(self)

    self.go_camera_geometry_buffer_ = nil
    self.go_camera_ssao_ = nil
    self.go_camera_deferred_rendering_ = nil

    ---@field render_texture_geometry_buffer_ RenderTextureGeometryBuffer
    self.render_texture_geometry_buffer_ = nil
    self.render_texture_ssao_ = nil
    self.noise_texture_ = nil
    self.go_skeleton_ = nil --骨骼蒙皮动画物体
    self.animation_ = nil--骨骼动画
    self.animation_clip_ = nil --- 骨骼动画片段
    self.material_fbx_model_ = nil --材质
    self.environment_=nil --环境
    self.go_point_light_1_=nil --灯光
    self.go_point_light_2_=nil --灯光
    self.go_ssao_deferred_rendering_plane_=nil--墙壁
    self.material_ssao_deferred_rendering_plane_=nil
end

function LoginScene:Awake()
    print("LoginScene Awake")
    LoginScene.super.Awake(self)

    self:CreateEnvironment()
    self:CreateDirectionalLight1()
    self:CreatePointLight1()
    self:CreatePointLight2()
    self:CreateModel()

    self:CreateGeometryBufferCamera()

    --self:CreateSSAOCamera()
    --self:CreateSSAOPlane()

    self:CreateSSAODeferredRenderingCamera()
    self:CreateSSAODeferredRenderingPlane()
end

--- 创建环境
function LoginScene:CreateEnvironment()
    self.environment_=Environment.new()
    self.environment_:set_ambient_color(glm.vec3(1.0,1.0,1.0))
    self.environment_:set_ambient_color_intensity(0.3)
end

--- 创建方向光1
function LoginScene:CreateDirectionalLight1()
    self.go_directional_light_1_= GameObject.new("directional_light_1")
    self.go_directional_light_1_:AddComponent(Transform)
    self.go_directional_light_1_:GetComponent(Transform):set_local_rotation(glm.vec3(0,0,0))

    local light=self.go_directional_light_1_:AddComponent(DirectionalLight)
    light:set_color(glm.vec3(1.0,1.0,1.0))
    light:set_intensity(1.0)
end

--- 创建点光源1
function LoginScene:CreatePointLight1()
    self.go_point_light_1_= GameObject.new("point_light_1")
    self.go_point_light_1_:AddComponent(Transform):set_local_position(glm.vec3(-2,0,5))
    ---@type PointLight
    local light=self.go_point_light_1_:AddComponent(PointLight)

    light:set_color(glm.vec3(1.0,0.0,0.0))
    light:set_intensity(1.0)
    light:set_attenuation_constant(1.0)
    light:set_attenuation_linear( 0.35)
    light:set_attenuation_quadratic( 0.44)
end

--- 创建点光源2
function LoginScene:CreatePointLight2()
    self.go_point_light_2_= GameObject.new("point_light_2")
    self.go_point_light_2_:AddComponent(Transform):set_local_position(glm.vec3(2,0,5))
    ---@type PointLight
    local light=self.go_point_light_2_:AddComponent(PointLight)

    light:set_color(glm.vec3(0.0,1.0,0.0))
    light:set_intensity(1.0)
    light:set_attenuation_constant(1.0)
    light:set_attenuation_linear( 0.35)
    light:set_attenuation_quadratic( 0.44)
end

--- 创建模型
function LoginScene:CreateModel()
    --创建骨骼蒙皮动画
    self.go_skeleton_=GameObject.new("skeleton")
    self.go_skeleton_:set_layer(1)
    self.go_skeleton_:AddComponent(Transform):set_local_position(glm.vec3(0, 0, 0))
    self.go_skeleton_:GetComponent(Transform):set_local_rotation(glm.vec3(0, 0, 0))
    local anim_clip_name="animation/fbx_extra_basic_plane_bones_basic_plane_bones_basic_plane_bones_armatureaction_basic_plane_.skeleton_anim"
    self.go_skeleton_:AddComponent(Animation):LoadAnimationClipFromFile(anim_clip_name,"idle")

    local mesh_filter=self.go_skeleton_:AddComponent(MeshFilter)
    mesh_filter:LoadMesh("model/basic_plane_model_basic_plane.mesh")--加载Mesh
    mesh_filter:LoadWeight("model/fbx_extra_basic_plane.weight")--加载权重文件

    --手动创建Material
    self.material_fbx_model_ = Material.new()--设置材质
    self.material_fbx_model_:Parse("material/basic_plane_ssao_gbuffer.mat")

    --挂上 MeshRenderer 组件
    local skinned_mesh_renderer= self.go_skeleton_:AddComponent(SkinnedMeshRenderer)
    skinned_mesh_renderer:SetMaterial(self.material_fbx_model_)

    --播放动画
    self.go_skeleton_:GetComponent(Animation):Play("idle")
end

--- 创建渲染到GeometryBuffer相机
function LoginScene:CreateGeometryBufferCamera()
    --创建相机1 GameObject
    self.go_camera_geometry_buffer_= GameObject.new("geometry_buffer_camera")
    --挂上 Transform 组件
    self.go_camera_geometry_buffer_:AddComponent(Transform):set_local_position(glm.vec3(0, 0, 10))
    self.go_camera_geometry_buffer_:GetComponent(Transform):set_local_rotation(glm.vec3(0, 0, 0))
    --挂上 Camera 组件
    local geometry_buffer_camera=self.go_camera_geometry_buffer_:AddComponent(Camera)
    --设置为黑色背景
    geometry_buffer_camera:set_clear_color(0,0,0,1)
    geometry_buffer_camera:set_depth(1)
    geometry_buffer_camera:set_culling_mask(1)
    geometry_buffer_camera:SetView(glm.vec3(0.0,0.0,0.0), glm.vec3(0.0,1.0,0.0))
    geometry_buffer_camera:SetPerspective(60, Screen:aspect_ratio(), 1, 1000)
    --设置延迟渲染
    geometry_buffer_camera:set_deferred_shading(true)
    --设置RenderTexture
    self.render_texture_geometry_buffer_ = RenderTextureGeometryBuffer.new()
    self.render_texture_geometry_buffer_:Init(960,640)
    geometry_buffer_camera:set_target_render_texture(self.render_texture_geometry_buffer_)
end

--- 创建渲染到SSAO相机
function LoginScene:CreateSSAOCamera()
    --创建相机1 GameObject
    self.go_camera_ssao_= GameObject.new("ssao_camera")
    --挂上 Transform 组件
    self.go_camera_ssao_:AddComponent(Transform):set_local_position(glm.vec3(0, 0, 10))
    self.go_camera_ssao_:GetComponent(Transform):set_local_rotation(glm.vec3(0, 0, 0))
    --挂上 Camera 组件
    local ssao_camera=self.go_camera_ssao_:AddComponent(Camera)
    --设置为黑色背景
    ssao_camera:set_clear_color(0,0,0,1)
    ssao_camera:set_depth(2)
    ssao_camera:set_culling_mask(2)
    ssao_camera:SetView(glm.vec3(0.0,0.0,0.0), glm.vec3(0.0,1.0,0.0))
    ssao_camera:SetPerspective(60, Screen:aspect_ratio(), 1, 1000)
    --设置RenderTexture
    self.render_texture_ssao_ = RenderTexture.new()
    self.render_texture_ssao_:Init(960,640)
    ssao_camera:set_target_render_texture(self.render_texture_ssao_)
end

---手动创建SSAO目标FBO需要的Plane
---@return void
function LoginScene:CreateSSAOPlane()
    local vertex_data={
        -1,-1,0,  1.0,1.0,1.0,1.0, 0,0, -1,-1,1,
        1,-1,0,  1.0,1.0,1.0,1.0, 1,0, 1,-1,1,
        1, 1,0,  1.0,1.0,1.0,1.0, 1,1, 1, 1,1,
        -1, 1,0,  1.0,1.0,1.0,1.0, 0,1, -1, 1,1,
    }
    local vertex_index_data={
        0,1,2,
        0,2,3,
    }

    local go_ssao_near_plane_=GameObject.new("ssao_near_plane")
    ObjectReferenceManager:Retain(go_ssao_near_plane_)

    go_ssao_near_plane_:AddComponent(Transform):set_local_position(glm.vec3(0, 0, -10))
    go_ssao_near_plane_:GetComponent(Transform):set_local_rotation(glm.vec3(0, 0, 0))

    go_ssao_near_plane_:set_layer(2)

    local mesh_filter=go_ssao_near_plane_:AddComponent(MeshFilter)
    mesh_filter:CreateMesh(vertex_data,vertex_index_data)--手动构建Mesh

    --手动创建Material
    self.material_ssao_near_plane_ = Material.new()
    self.material_ssao_near_plane_:Parse("material/renderer_to_ssao_buffer.mat")
    --传入GBuffer的纹理
    self.material_ssao_near_plane_:SetTexture("u_frag_position_texture",self.render_texture_geometry_buffer_:frag_position_texture_2d())
    self.material_ssao_near_plane_:SetTexture("u_frag_normal_texture",self.render_texture_geometry_buffer_:frag_normal_texture_2d())
    --创建SSAONoiseTexture，传入噪声纹理
    local noise = self:CreateNoise()
    local noise_texture = NoiseTexture.new()
    noise_texture:Init(4, 4, noise)
    self.material_ssao_near_plane_:SetTexture("u_noise_texture",noise_texture:noise_texture_2d())
    ObjectReferenceManager:Retain(noise_texture)

    --挂上 MeshRenderer 组件
    local mesh_renderer= go_ssao_near_plane_:AddComponent(MeshRenderer)
    mesh_renderer:SetMaterial(self.material_ssao_near_plane_)
end

---创建16个随机向量，用于噪声纹理
---@return table<number,glm.vec3>
function LoginScene:CreateNoise()
    local ssaoNoise = {}
    for i = 1, 16 do
        local noise = glm.vec3(
                math.random_floats(0,1) * 2.0 - 1.0,
                math.random_floats(0,1) * 2.0 - 1.0,
                0.0)
        table.insert(ssaoNoise, noise)
    end
    return ssaoNoise
end


--- 创建延迟渲染相机
function LoginScene:CreateSSAODeferredRenderingCamera()
    --创建相机1 GameObject
    self.go_camera_deferred_rendering_= GameObject.new("ssao_deferred_rendering_camera")
    --挂上 Transform 组件
    self.go_camera_deferred_rendering_:AddComponent(Transform):set_local_position(glm.vec3(0, 0, 10))
    self.go_camera_deferred_rendering_:GetComponent(Transform):set_local_rotation(glm.vec3(0, 0, 0))
    --挂上 Camera 组件
    local camera_deferred_rendering=self.go_camera_deferred_rendering_:AddComponent(Camera)
    --设置为黑色背景
    camera_deferred_rendering:set_clear_color(0,0,0,1)
    camera_deferred_rendering:set_depth(3)
    camera_deferred_rendering:set_culling_mask(3)
    camera_deferred_rendering:SetView(glm.vec3(0.0,0.0,0.0), glm.vec3(0.0,1.0,0.0))
    camera_deferred_rendering:SetPerspective(60, Screen:aspect_ratio(), 1, 1000)
end

---手动创建Mesh
---@return void
function LoginScene:CreateSSAODeferredRenderingPlane()
    local vertex_data={
        -1,-1,0,  1.0,1.0,1.0,1.0, 0,0, -1,-1,1,
        1,-1,0,  1.0,1.0,1.0,1.0, 1,0, 1,-1,1,
        1, 1,0,  1.0,1.0,1.0,1.0, 1,1, 1, 1,1,
        -1, 1,0,  1.0,1.0,1.0,1.0, 0,1, -1, 1,1,
    }
    local vertex_index_data={
        0,1,2,
        0,2,3,
    }

    self.go_ssao_deferred_rendering_plane_=GameObject.new("ssao_deferred_rendering_plane")
    self.go_ssao_deferred_rendering_plane_:AddComponent(Transform):set_local_position(glm.vec3(0, 0, -10))
    self.go_ssao_deferred_rendering_plane_:GetComponent(Transform):set_local_rotation(glm.vec3(0, 0, 0))

    self.go_ssao_deferred_rendering_plane_:set_layer(3)

    local mesh_filter=self.go_ssao_deferred_rendering_plane_:AddComponent(MeshFilter)
    mesh_filter:CreateMesh(vertex_data,vertex_index_data)--手动构建Mesh

    --手动创建Material
    self.material_ssao_deferred_rendering_plane_ = Material.new()--设置材质
    self.material_ssao_deferred_rendering_plane_:Parse("material/ssao_deferred_rendering.mat")
    --设置材质纹理
    self.material_ssao_deferred_rendering_plane_:SetTexture("u_frag_position_texture",self.render_texture_geometry_buffer_:frag_position_texture_2d())
    self.material_ssao_deferred_rendering_plane_:SetTexture("u_frag_normal_texture",self.render_texture_geometry_buffer_:frag_normal_texture_2d())
    self.material_ssao_deferred_rendering_plane_:SetTexture("u_frag_vertex_color_texture",self.render_texture_geometry_buffer_:frag_vertex_color_texture_2d())
    self.material_ssao_deferred_rendering_plane_:SetTexture("u_frag_diffuse_color_texture",self.render_texture_geometry_buffer_:frag_diffuse_color_texture_2d())
    self.material_ssao_deferred_rendering_plane_:SetTexture("u_frag_specular_intensity_texture",self.render_texture_geometry_buffer_:frag_specular_intensity_texture_2d())
    self.material_ssao_deferred_rendering_plane_:SetTexture("u_frag_specular_highlight_shininess_texture",self.render_texture_geometry_buffer_:frag_specular_highlight_shininess_texture_2d())
    --self.material_ssao_deferred_rendering_plane_:SetTexture("u_ssao_texture",self.render_texture_ssao_:color_texture_2d())

    --挂上 MeshRenderer 组件
    local mesh_renderer= self.go_ssao_deferred_rendering_plane_:AddComponent(MeshRenderer)
    mesh_renderer:SetMaterial(self.material_ssao_deferred_rendering_plane_)
end


---创建SSAOKernel即对片段周围随机采样点，对于每个片段，会再叠加一个随机值。在Shader中有个64位长度的数组，需要一个一个将数值上传到Shader中。
---@return table<number,glm.vec3>
function LoginScene:GenerateSSAOKernel()
    local lerp = function(a, b, f)
        return a + f * (b - a)
    end

    local ssaoKernel = {}
    math.randomseed(os.time())

    for i = 1, 64 do
        local sample = glm.vec3(
                math.random_floats(-1.0, 1.0),
                math.random_floats(-1.0, 1.0),
                math.random_floats(0.0, 1.0)
        )
        sample = glm.normalize(sample)
        sample = sample * math.random_floats(0.0, 1.0)

        local scale = i / 64.0
        scale = lerp(0.1, 1.0, scale * scale)
        sample = sample * scale

        table.insert(ssaoKernel, sample)

        --print("ssaoKernel[" .. i .. "]:" .. tostring(sample))
    end

    return ssaoKernel
end




function LoginScene:Update()
    --print("LoginScene:Update")
    LoginScene.super.Update(self)

    --设置观察者世界坐标(即相机位置)
    local camera_position=self.go_camera_geometry_buffer_:GetComponent(Transform):position()
    self.material_fbx_model_:SetUniform3f("u_view_pos",camera_position)
    --设置物体反射度、高光强度
    self.material_fbx_model_:SetUniform1f("u_specular_highlight_shininess",32.0)
    --设置远近平面
    --self.material_fbx_model_:SetUniform1f("near_plane",1.0)
    --self.material_fbx_model_:SetUniform1f("far_plane",1000.0)

    --设置ssao_kernel
    --local ssao_kernel=self:GenerateSSAOKernel()
    --for i=1,#ssao_kernel do
    --    self.material_ssao_near_plane_:SetUniform3f("u_ssao_kernel["..(i-1).."]",ssao_kernel[i])
    --end

    --鼠标滚轮控制相机远近
    self.go_camera_geometry_buffer_:GetComponent(Transform):set_local_position(self.go_camera_geometry_buffer_:GetComponent(Transform):position() *(10 - Input.mouse_scroll())/10)

    --旋转相机
    if Input.GetKeyDown(Cpp.KeyCode.KEY_CODE_LEFT_ALT) and Input.GetMouseButtonDown(Cpp.KeyCode.MOUSE_BUTTON_LEFT) and self.last_frame_mouse_position_ then
        --print(Input.mousePosition(),self.last_frame_mouse_position_)
        local degrees= Input.mousePosition().x - self.last_frame_mouse_position_.x
        self.last_frame_mouse_position_=Input.mousePosition()

        local old_mat4=glm.mat4(1.0)
        local rotate_mat4=glm.rotate(old_mat4,glm.radians(-degrees),glm.vec3(0.0,1.0,0.0))--以相机所在坐标系位置，计算用于旋转的矩阵，这里是零点，所以直接用方阵。

        local camera_pos=self.go_camera_geometry_buffer_:GetComponent(Transform):local_position()
        local old_pos=glm.vec4(camera_pos.x,camera_pos.y,camera_pos.z,1.0)
        local new_pos=rotate_mat4*old_pos--旋转矩阵 * 原来的坐标 = 相机以零点做旋转。
        --print(new_pos)
        self.go_camera_geometry_buffer_:GetComponent(Transform):set_local_position(glm.vec3(new_pos.x,new_pos.y,new_pos.z))
    end
    self.last_frame_mouse_position_=Input.mousePosition()
end