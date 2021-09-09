LoginScene={
    transform_camera_1_,
    camera_1_,
    last_frame_mouse_position_,--上一帧的鼠标位置
    transform_player_= nullptr,
    AudioStudioEvent* audio_studio_event_
}

-- public:
function LoginScene:Awake()
    print("LoginScene:Awake")
end

function LoginScene:Update()
    print("LoginScene:Update")
end

-- private:
--- 创建音源
function LoginScene:CreateAudioSource()

end

--- 创建听者
function LoginScene:CreateAudioListener()

end