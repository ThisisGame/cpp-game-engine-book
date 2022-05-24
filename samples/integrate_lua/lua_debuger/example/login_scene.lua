require("lua_extension")
require("component")
require("game_object")

LoginScene=class("LoginScene",Component)

--- 登录场景
---@class LoginScene
function LoginScene:ctor()
    LoginScene.super.ctor(self)
end

function LoginScene:Awake()
    print("LoginScene Awake")
end

function LoginScene:Update()
    --print("LoginScene Update")
end