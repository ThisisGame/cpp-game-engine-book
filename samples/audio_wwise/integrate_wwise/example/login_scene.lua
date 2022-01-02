LoginScene={
    listener_go_,--Listener
    bgm_go_,--背景音乐
    hero_go_,--英雄
    monster_go_,--怪物


    monster_pos_,--怪物位置，屏幕分为左中右三个区域，怪物在左中右区域中随机出现。
    hero_pos_index_,--英雄位置
    hero_pos_array_,--英雄位置数组
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
    WwiseAudio.LoadBank("game.bnk")

    --创建Listener
    self.listener_go_=GameObject("listener")
    self.listener_go_:AddComponent("Transform")
    self.listener_go_:AddComponent("AudioListener")
    self.listener_go_:GetComponent("Transform"):set_position(glm.vec3(0,0,0))

    --创建英雄
    self.hero_go_=GameObject("hero")
    self.hero_go_:AddComponent("Transform")
    self.hero_go_:AddComponent("AudioSource"):SetEvent("fire")

    -- 创建背景音乐
    self.bgm_go_=GameObject("bgm")
    self.bgm_go_:AddComponent("Transform")
    --self.bgm_go_:AddComponent("AudioSource"):SetEvent("open_door_walk_in")
    --self.bgm_go_:GetComponent("AudioSource"):Play()
    --self.bgm_go_:GetComponent("Transform"):set_position(glm.vec3(0,0,0))

    -- 创建怪物
    self.monster_go_=GameObject("monster")
    self.monster_go_:AddComponent("Transform")

    -- 英雄默认在中间
    self.hero_pos_index_=0

    self:MonsterRefresh()
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
    if Input.GetKeyUp(KeyCode.KEY_CODE_SPACE) then
        print("LoginScene:Update KEY_CODE_SPACE monster_pos_:" .. tostring(self.monster_pos_) .. " hero_pos_index_:" .. tostring(self.hero_pos_index_))
        self.hero_go_:GetComponent("AudioSource"):Play()

        -- 判断英雄位置和怪物位置，如果相同，则播放击中音效
        if self.hero_pos_index_==self.monster_pos_ then
            print("LoginScene:Update KEY_CODE_SPACE hit")
            self:MonsterRefresh()
        end
    end

    if Input.GetKeyUp(KeyCode.KEY_CODE_A) or Input.GetKeyUp(KeyCode.KEY_CODE_LEFT) then
        if self.hero_pos_index_ > -1 then
            self.hero_pos_index_=self.hero_pos_index_-1
            self:SetHeroPosition()
        else
            print("LoginScene:Update KEY_CODE_A Move Left out of range")
        end

        print("LoginScene:Update KEY_CODE_A Move Left, hero_pos_index_:" .. tostring(self.hero_pos_index_))
    end

    if Input.GetKeyUp(KeyCode.KEY_CODE_D) or Input.GetKeyUp(KeyCode.KEY_CODE_RIGHT) then
        if self.hero_pos_index_ < 1 then
            self.hero_pos_index_=self.hero_pos_index_+1
            self:SetHeroPosition()
        else
            print("LoginScene:Update KEY_CODE_D Move Right out of range")
        end

        print("LoginScene:Update KEY_CODE_D Move Right, hero_pos_index_:" .. tostring(self.hero_pos_index_))
    end
end


-- 刷新怪物，更新怪物位置
function LoginScene:MonsterRefresh()
    -- 怪物位置
    self.monster_pos_=math.random(-1,1)
    print("MonsterRefresh monster_pos_:"..self.monster_pos_)
end

function LoginScene:SetHeroPosition()
    local x=self.hero_pos_index_ * 50
    self.hero_go_:GetComponent("Transform"):set_position(glm.vec3(x,0,0))
end