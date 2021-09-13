LoginScene={
    transform_camera_1_,
    camera_1_,
    last_frame_mouse_position_,--上一帧的鼠标位置
    transform_player_= nullptr,
    AudioStudioEvent* audio_studio_event_
}

-- public:
function LoginScene:Awake()
    --创建相机1 GameObject
    local go_camera_1= GameObject("main_camera")
    --挂上 Transform 组件
    transform_camera_1_=go_camera_1:AddComponent("Transform")
    transform_camera_1_:set_position(glm.vec3(0, 0, 10))
    --挂上 Camera 组件
    camera_1_=go_camera_1:AddComponent("Camera")
    camera_1_:set_depth(0)

    last_frame_mouse_position_=Input.mousePosition()

    self:CreateAudioSource()
    self:CreateAudioListener()
end

function LoginScene:CreateAudioSource() {
    local go= GameObject("audio_source_bgm")
    --挂上 Transform 组件
    local transform = go:AddComponent("Transform")
    --挂上 MeshFilter 组件
    local mesh_filter= go:AddComponent("MeshFilter")
    mesh_filter:LoadMesh("model/sphere.mesh")
    --挂上 MeshRenderer 组件
    local mesh_renderer= go:AddComponent("MeshRenderer")
    local material = Material()--设置材质
    material:Parse("material/sphere_audio_source_3d_music.mat")
    mesh_renderer:SetMaterial(material)

    --加载bank
    AudioStudio.LoadBankFile("audio/test.bank")
    AudioStudio.LoadBankFile("audio/test.strings.bank")
    audio_studio_event_=AudioStudio.CreateEventInstance("event:/footstep")
}

function LoginScene:CreateAudioListener() {
    local go= GameObject("Player")
    transform_player_ =dynamic_cast<Transform*>(go:AddComponent("Transform"))
    transform_player_:set_position(2.0,0.0,0.0)
    local mesh_filter=dynamic_cast<MeshFilter*>(go:AddComponent("MeshFilter"))
    mesh_filter:LoadMesh("model/sphere.mesh")
    local mesh_renderer=dynamic_cast<MeshRenderer*>(go:AddComponent("MeshRenderer"))
    local material = Material()--设置材质
    material:Parse("material/sphere_audio_source_3d_listener.mat")
    mesh_renderer:SetMaterial(material)

    --设置听者位置
    AudioStudio.setListenerAttributes(0,0,0)
}


function LoginScene:Update() {
    camera_1_:SetView(glm.vec3(0, 0, 0), glm.vec3(0, 1, 0))
    camera_1_:SetProjection(60.f, Screen::aspect_ratio(), 1.f, 1000.f)

    --旋转相机
    if Input.GetKeyDown(KEY_CODE_LEFT_ALT) and Input.GetMouseButtonDown(MOUSE_BUTTON_LEFT)) then
        float degrees= Input.mousePosition().x_ - last_frame_mouse_position_.x_

        glm.mat4 old_mat4=glm.mat4(1.0f)

        glm.mat4 rotate_mat4=glm.rotate(old_mat4,glm.radians(degrees),glm.vec3(0.0f,1.0f,0.0f))--以相机所在坐标系位置，计算用于旋转的矩阵，这里是零点，所以直接用方阵。
        glm.vec4 old_pos=glm.vec4(transform_camera_1_:position(),1.0f)
        glm.vec4 new_pos=rotate_mat4*old_pos--旋转矩阵 * 原来的坐标 = 相机以零点做旋转。
        std::cout<<glm.to_string(new_pos)<<std::endl

        transform_camera_1_:set_position(glm.vec3(new_pos))
    end

    --播放Event实例
    if Input.GetKeyUp(KEY_CODE_S)) then
        audio_studio_event_:Start()
    end

    --按键盘1、2、3设置参数值，切换不同的地面类型，播放不同的脚步声
    if Input.GetKeyUp(KEY_CODE_1)) then
        audio_studio_event_:SetParameterByName("groundtype",0.0f)
    else if Input.GetKeyUp(KEY_CODE_2) then
        audio_studio_event_:SetParameterByName("groundtype",1.0f)
    else if Input.GetKeyUp(KEY_CODE_3) then
        audio_studio_event_:SetParameterByName("groundtype",2.0f)
    end

    last_frame_mouse_position_=Input.mousePosition()

    --鼠标滚轮控制相机远近
    transform_camera_1_:set_position(transform_camera_1_:position() *(10 - Input.mouse_scroll())/10.f)

    --设置听者位置
    glm.mat4 rotate_mat4=glm.rotate(glm.mat4(1.0f),glm.radians(Time::delta_time()*60),glm.vec3(0.0f,0.0f,1.0f))
    glm.vec4 old_pos=glm.vec4(transform_player_:position(),1.0f)
    glm.vec4 new_pos=rotate_mat4*old_pos--旋转矩阵 * 原来的坐标 = 以零点做旋转。
    transform_player_:set_position(glm.vec3(new_pos))
    local player_pos=transform_player_:position()
    AudioStudio.setListenerAttributes(player_pos.x,player_pos.y,player_pos.z)
}