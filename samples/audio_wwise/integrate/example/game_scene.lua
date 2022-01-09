GameScene={
    go_camera_ui_,--摄像机UI
    go_ui_image_,--UI图片

    listener_go_,--Listener
    monster_go_,--怪物
}

setmetatable(GameScene,{["__call"]=function(table,param)
    local instance=setmetatable({},{__index=table})
    return instance
end})


-- public:
function GameScene:Awake()
    -- 加载Init.bank文件
    WwiseAudio.LoadBank("Init.bnk")
    -- 加载游戏音效bank文件
    WwiseAudio.LoadBank("fight.bnk")

    --创建Listener
    self.listener_go_=GameObject("listener")
    self.listener_go_:AddComponent("Transform")
    self.listener_go_:AddComponent("AudioListener")
    self.listener_go_:GetComponent("Transform"):set_position(glm.vec3(0,0,0))

    -- 创建怪物
    self.monster_go_=GameObject("monster")
    self.monster_go_:AddComponent("Transform")
    self.monster_go_:AddComponent("AudioSource")
    self.monster_go_:GetComponent("AudioSource"):SetEvent("die")
    self.monster_go_:GetComponent("AudioSource"):set_event_end_callback(function()
        print("audio monster die end")
    end)

    self:CreateUI()
end


function GameScene:game_object()
    print("GameScene:game_object")
    return self.game_object_
end

function GameScene:set_game_object(game_object)
    print("GameScene:set_game_object " .. tostring(game_object) .. " self:" .. tostring(self))
    self.game_object_=game_object
end

function GameScene:CreateUI()
    -- 创建UI相机 GameObject
    self.go_camera_ui_=GameObject("ui_camera")
    -- 挂上 Transform 组件
    local transform_camera_ui=self.go_camera_ui_:AddComponent("Transform")
    transform_camera_ui:set_position(glm.vec3(0, 0, 10))
    -- 挂上 Camera 组件
    local camera_ui=self.go_camera_ui_:AddComponent("UICamera")
    -- 设置正交相机
    camera_ui:SetView(glm.vec3(0, 0, 0), glm.vec3(0, 1, 0))
    camera_ui:SetOrthographic(-Screen.width()/2,Screen.width()/2,-Screen.height()/2,Screen.height()/2,-100,100)

    -- 创建 UIImage
    self.go_ui_image_=GameObject("image")
    self.go_ui_image_:AddComponent("Transform"):set_position(glm.vec3(-480, -320, 0))
    -- 挂上 UIImage 组件
    local ui_image_mod_bag=self.go_ui_image_:AddComponent("UIImage")
    ui_image_mod_bag:set_texture(Texture2D.LoadFromFile("images/need_head_phone.cpt"))
end

function GameScene:Update()
    --print("GameScene:Update")
    if Input.GetKeyUp(KeyCode.KEY_CODE_SPACE) then
        print("GameScene:Update KEY_CODE_SPACE")
        self.monster_go_:GetComponent("AudioSource"):Play()
    end
end