require("lua_extension")

Player=class("Player",Component)

--- 角色 脚本
---@class Player
function Player:ctor()
    Player.super.ctor(self)
end

function Player:Awake()
    print("Player Awake")
    Player.super.Awake(self)
end

function Player:Update()
    Player.super.Update(self)
end

function Player:OnTriggerEnter(game_object)
    Player.super.OnTriggerEnter(self,game_object)
    print("Player OnTriggerEnter,game_object name:",game_object:name())
end

function Player:OnTriggerExit(game_object)
    Player.super.OnTriggerExit(self,game_object)
    print("Player OnTriggerExit,game_object name:",game_object:name())
end