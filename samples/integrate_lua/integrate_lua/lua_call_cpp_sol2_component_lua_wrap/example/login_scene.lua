require("lua_extension")
require("component")
require("game_object")

--- 登录场景
---@class LoginScene
function LoginScene:ctor()
    self.game_object_=nil
end

function LoginScene:game_object()
    return self.game_object_
end

function LoginScene:set_game_object(game_object)
    self.game_object_=game_object
end

function LoginScene:Awake()
    print("LoginScene Awake")
end

function LoginScene:Update()
    print("LoginScene Update")
end