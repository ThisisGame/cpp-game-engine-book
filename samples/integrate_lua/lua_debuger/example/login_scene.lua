require("lua_extension")
require("component")
require("game_object")

LoginScene=class("LoginScene",Component)

--- 登录场景
---@class LoginScene
function LoginScene:ctor()
    LoginScene.super.ctor(self)
    self.app_version_="1.0.0"
end

function LoginScene:Awake()
    LoginScene.super.Awake(self)
    print("LoginScene Awake,app_version:",self.app_version_)
end

function LoginScene:Update()
    LoginScene.super.Update(self)
    print("LoginScene Update")
end