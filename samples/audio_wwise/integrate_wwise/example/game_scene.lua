GameScene={
    second_callback_call_time_ = 0,--每秒回调调用的时间
    listener_go_,--Listener
    bgm_go_,--背景音乐
    hero_go_,--英雄
    monster_go_,--怪物

    hero_pos_index_,--英雄位置
    hero_pos_array_,--英雄位置数组

    monster_pos_index_,--怪物位置，屏幕分为左中右三个区域，怪物在左中右区域中随机出现。
    monster_pos_array_,--怪物位置数组

    monster_infos_,--怪物信息
    monster_info_index_,--当前怪物信息index

    is_monster_die_=false,--是否怪物死亡
    monster_die_time_=0,--怪物死亡时间
    refresh_monster_delay_timer_=4,--刷新怪物定时器，怪物死亡后，等几秒钟再刷出新的怪物，这段时间内播放走路的声音。

    hero_fire_time_=0,--英雄开火时间
    hero_fire_cd_=5,--英雄开火CD
}

setmetatable(GameScene,{["__call"]=function(table,param)
    local instance=setmetatable({},{__index=table})
    return instance
end})

-- public:
function GameScene:Awake()
    self.hero_pos_array_={
        glm.vec3(-5,0,10),
        glm.vec3(0,0,10),
        glm.vec3(5,0,10),
    }
    self.monster_pos_array_={
        glm.vec3(-5,0,10),
        glm.vec3(0,0,10),
        glm.vec3(5,0,10),
    }
    self.monster_infos_={
        {name="lion",audio="lion",die_audio="middle_die"},
        {name="wolf",audio="wolf",die_audio="middle_die"},
        {name="orangutan",audio="orangutan",die_audio="big_die"},
        {name="peg",audio="peg",die_audio="little_die"},
        {name="sheep",audio="sheep",die_audio="little_die"},
        {name="tiger",audio="tiger",die_audio="middle_die"},
        {name="horse",audio="horse",die_audio="big_die"}
    }

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
    self.hero_go_:GetComponent("AudioSource"):set_event_end_callback(function()
        print("fire end")
    end)

    -- 创建背景音乐
    self.bgm_go_=GameObject("bgm")
    self.bgm_go_:AddComponent("Transform")
    self.bgm_go_:AddComponent("AudioSource"):SetEvent("open_door_walk_in")
    self.bgm_go_:GetComponent("AudioSource"):set_event_end_callback(function()
        print("bgm end")
        self:InitGame()
    end)
    self.bgm_go_:GetComponent("AudioSource"):Play()

    -- 创建怪物
    self.monster_go_=GameObject("monster")
    self.monster_go_:AddComponent("Transform")
    self.monster_go_:AddComponent("AudioSource")

    -- 英雄默认在中间
    self.hero_pos_index_=2
end


function GameScene:game_object()
    print("GameScene:game_object")
    return self.game_object_
end

function GameScene:set_game_object(game_object)
    print("GameScene:set_game_object " .. tostring(game_object) .. " self:" .. tostring(self))
    self.game_object_=game_object
end

function GameScene:InitGame()
    self:MonsterRefresh()
    self:SetHeroPosition()
end


function GameScene:Update()
    --print("GameScene:Update")
    -- 每秒回调
    if Time.TimeSinceStartup() - self.second_callback_call_time_ >=1 then
        self.second_callback_call_time_=Time.TimeSinceStartup()
        self:SecondCallback()
    end


    if Input.GetKeyUp(KeyCode.KEY_CODE_SPACE) then
        print("Time.TimeSinceStartup() " .. tostring(Time.TimeSinceStartup()))
        if Time.TimeSinceStartup() - self.hero_fire_time_ < self.hero_fire_cd_ then
            print("fire cd")
            return
        end
        print("GameScene:Update KEY_CODE_SPACE monster_pos_index_:" .. tostring(self.monster_pos_index_) .. " hero_pos_index_:" .. tostring(self.hero_pos_index_))
        --播放开火音效
        self.hero_go_:GetComponent("AudioSource"):Play()
        --记录英雄开火时间
        self.hero_fire_time_=Time.TimeSinceStartup()

        -- 判断英雄位置和怪物位置，如果相同，则播放击中音效
        if self.hero_pos_index_==self.monster_pos_index_ then
            print("GameScene:Update KEY_CODE_SPACE hit")
            self:PlayMonsterDieAudio()
            self.is_monster_die_=true
            self.monster_die_time_=Time.TimeSinceStartup()
        end
    end

    if Input.GetKeyUp(KeyCode.KEY_CODE_A) or Input.GetKeyUp(KeyCode.KEY_CODE_LEFT) then
        if self.hero_pos_index_ > 1 then
            self.hero_pos_index_=self.hero_pos_index_-1
            self:SetHeroPosition()
        else
            print("GameScene:Update KEY_CODE_A Move Left out of range")
        end

        print("GameScene:Update KEY_CODE_A Move Left, hero_pos_index_:" .. tostring(self.hero_pos_index_))
    end

    if Input.GetKeyUp(KeyCode.KEY_CODE_D) or Input.GetKeyUp(KeyCode.KEY_CODE_RIGHT) then
        if self.hero_pos_index_ < 3 then
            self.hero_pos_index_=self.hero_pos_index_+1
            self:SetHeroPosition()
        else
            print("GameScene:Update KEY_CODE_D Move Right out of range")
        end

        print("GameScene:Update KEY_CODE_D Move Right, hero_pos_index_:" .. tostring(self.hero_pos_index_))
    end
end

function GameScene:SecondCallback()
    print("self.is_monster_die_:" .. tostring(self.is_monster_die_) .. " self.monster_die_time_:" .. tostring(self.monster_die_time_) .. " Time.TimeSinceStartup():" .. tostring(Time.TimeSinceStartup()))
    -- 计时刷新怪物
    if self.is_monster_die_ and (Time.TimeSinceStartup() - self.monster_die_time_ >=self.refresh_monster_delay_timer_) then
        print("refresh monster")
        self.monster_die_time_=0
        self.is_monster_die_=false
        self:MonsterRefresh()
    end
end


-- 刷新怪物，更新怪物位置
function GameScene:MonsterRefresh()
    -- 怪物位置
    self.monster_pos_index_=math.random(1,3)
    print("MonsterRefresh monster_pos_index_:"..self.monster_pos_index_)

    local pos=self.monster_pos_array_[self.monster_pos_index_]
    self.monster_go_:GetComponent("Transform"):set_position(pos)
    print("MonsterRefresh " .. tostring(pos))

    -- 当前怪物信息index
    self.monster_info_index_=math.random(1,#self.monster_infos_)
    local monster_info=self.monster_infos_[self.monster_info_index_]
    print("MonsterRefresh " .. monster_info.audio)
    self.monster_go_:GetComponent("AudioSource"):SetEvent(monster_info.audio)
    self.monster_go_:GetComponent("AudioSource"):Play()
end

function GameScene:PlayMonsterDieAudio()
    local monster_info=self.monster_infos_[self.monster_info_index_]
    print("PlayMonsterDieAudio " .. monster_info.die_audio)
    self.monster_go_:GetComponent("AudioSource"):Stop()
    self.monster_go_:GetComponent("AudioSource"):SetEvent(monster_info.die_audio)
    self.monster_go_:GetComponent("AudioSource"):Play()
end

function GameScene:SetHeroPosition()
    local pos=self.hero_pos_array_[self.hero_pos_index_]
    self.hero_go_:GetComponent("Transform"):set_position(pos)
    print("SetHeroPosition " .. tostring(pos))
end