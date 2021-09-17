LoginScene={
    game_object_
}

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

setmetatable(LoginScene,{["__call"]=function(table,param)
    local instance=setmetatable({},{__index=table})
    return instance
end})


function main()
    local game_object = GameObject()
    print("game_object:" .. tostring(game_object))

    print("----------- Add Cpp.Component.Animator --------------")

    local animator=game_object:AddComponent("Animator")
    print("add animator:" .. tostring(animator))

    print("----------- Add Cpp.Component.Camera ----------------")

    local camera=game_object:AddComponent("Camera")
    print("add camera:" .. tostring(camera))

    print("----------- Add Lua.Component.LoginScene ------------")

    local loginScene=game_object:AddComponent("LoginScene")
    print("add loginScene:" .. tostring(loginScene))

    print("----------- Get Lua.Component.LoginScene ------------")
    local loginScene=game_object:GetComponent("LoginScene")
    print("get loginScene:" .. tostring(loginScene))

    print("----------- Get Cpp.Component.Camera ------------")
    local camera=loginScene:game_object():GetComponent("Camera")
    print("get camera:" .. tostring(camera))

    print("----------- Get Cpp.Component.Animator ------------")
    local animator=camera:game_object():GetComponent("Animator")
    print("get animator:" .. tostring(animator))

    print("----------- Cpp.glm.vec3 ------------")
    local position=glm.vec3(1,2,3)
    position.x=11
    position.y=22
    position.z=33
    print(tostring(position))
    camera:set_position(glm.vec3(4,5,6))
    print(camera:position())
    print("-----------------------")

end