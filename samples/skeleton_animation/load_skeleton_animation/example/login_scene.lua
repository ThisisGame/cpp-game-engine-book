require("lua_extension")
require("renderer/animation_clip")

LoginScene=class("LoginScene",Component)

--- 登录场景
---@class LoginScene
function LoginScene:ctor()
    LoginScene.super.ctor(self)
    ---@type AnimationClip
    self.animation_clip_=nil
end

function LoginScene:Awake()
    print("LoginScene Awake")
    LoginScene.super.Awake(self)
    self.animation_clip_=AnimationClip.new()
    self.animation_clip_:LoadFromFile("animation/export.skeleton_anim")
    self.animation_clip_:Play()
end

function LoginScene:Update()
    --print("LoginScene Update")
    LoginScene.super.Update(self)
    self.animation_clip_:Update()
end