require("lua_extension")
require("audio/studio/audio_studio")

Player=class("Player",Component)

--- 角色 脚本
--- @class Player
function Player:ctor()
    Player.super.ctor(self)
    ---@field is_crash_ boolean @是否已经坠毁
    self.is_crash_=false
    self.audio_studio_event_warning_=nil
    self.audio_studio_event_crash_=nil
end

function Player:Awake()
    print("Player Awake")
    Player.super.Awake(self)

    self:CreateAudio()
end

function Player:CreateAudio()
    --设置听者位置
    AudioStudio:SetListenerAttributes(0,0,0)

    --加载bank
    AudioStudio:LoadBankFile("audio/integrate_physx.bank")
    AudioStudio:LoadBankFile("audio/integrate_physx.strings.bank")
    self.audio_studio_event_warning_=AudioStudio:CreateEventInstance("event:/crash_warning")
    self.audio_studio_event_warning_:Start()

    self.audio_studio_event_crash_=AudioStudio:CreateEventInstance("event:/crash")
end

function Player:Update()
    Player.super.Update(self)
    local position=self:game_object():GetComponent(Transform):position()
    local origin=position+glm.vec3(0,-1.1,0)
    --持续射线检测飞机触地
    local raycast_hit=RaycastHit.new();
    if Physics:RaycastSingle(origin,glm.vec3(0,-1,0),20,raycast_hit) then
        local hit_position=raycast_hit:position()
        --计算与地面距离
        local distance=glm.distance(origin,hit_position)
        if distance>1.1 then
            print("player raycast hit game_object:",raycast_hit:game_object():name()," pos:",hit_position)
        else
            if self.is_crash_ then
                return
            end
            print("player crash")
            self.is_crash_=true
            self.audio_studio_event_warning_:Stop()
            self.audio_studio_event_crash_:Start()
        end
    end
end

function Player:OnTriggerEnter(game_object)
    Player.super.OnTriggerEnter(self,game_object)
    print("Player OnTriggerEnter,game_object name:",game_object:name())
end

function Player:OnTriggerExit(game_object)
    Player.super.OnTriggerExit(self,game_object)
    print("Player OnTriggerExit,game_object name:",game_object:name())
end