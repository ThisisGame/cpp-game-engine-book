LoginScene={
    bgm_go_,--背景音乐
    hero_go_,--英雄
}

setmetatable(LoginScene,{["__call"]=function(table,param)
    local instance=setmetatable({},{__index=table})
    return instance
end})

-- public:
function LoginScene:Awake()
    -- 加载Init.bank文件
    WwiseAudio.LoadBank("Init.bnk")
    -- 加载游戏音效bank文件
    WwiseAudio.LoadBank("Car.bnk")

    --创建英雄
    self.hero_go_=GameObject("hero")
    self.hero_go_:AddComponent("Transform")
    self.hero_go_:AddComponent("AudioListener")

    -- 创建背景音乐
    self.bgm_go_=GameObject("hero")
    self.bgm_go_:AddComponent("Transform")
    self.bgm_go_:AddComponent("AudioSource"):SetEvent("Play_Engine")
    self.bgm_go_:GetComponent("AudioSource"):Play()
    self.bgm_go_:GetComponent("AudioSource"):SetRTPCValue("RPM",5000)
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

end