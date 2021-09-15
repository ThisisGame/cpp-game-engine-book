print("-------run lua--------")


local metatable={}
metatable["__call"]= function(table,param)
    print("__call table:" .. tostring(table) .. " param:" .. tostring(param))
    local instance=setmetatable({},{__index=table})
    return instance
end

LoginScene={
    game_object_=nil
}
setmetatable(LoginScene,metatable)
print("LoginScene:" .. tostring(LoginScene))

function LoginScene:game_object()
    return self.game_object_
end

function LoginScene:set_game_object(game_object)
    print("LoginScene:set_game_object:" .. tostring(game_object))
    self.game_object_=game_object
end

function LoginScene:Awake()
    print("LoginScene Awake")
end

function LoginScene:Update()
    print("LoginScene Update")
end


function main()
    local game_object = GameObject()
    print("game_object:" .. tostring(game_object))

    --print("---------------------------")
    --
    --animator=game_object:AddComponent("Animator")
    --print("animator:" .. tostring(animator))
    --animator:Awake()
    --
    --game_object=animator:game_object()
    --print("animator:game_object():" .. tostring(game_object))
    --
    --print("---------------------------")
    --
    --local camera=game_object:AddComponent("Camera")
    --print("camera:" .. tostring(camera))
    --camera:Awake()
    --
    --game_object=camera:game_object()
    --print("camera:game_object():" .. tostring(game_object))

    print("---------------------------")

    local loginScene=game_object:AddComponent("LoginScene")
    print("loginScene:" .. tostring(loginScene))
    loginScene:Awake()

    game_object=loginScene:game_object()
    print("loginScene:game_object():" .. tostring(game_object))
end

function set_game_object(game_object)
    print("set_game_object:" .. tostring(game_object))

    print("---------------------------")

    local animator=game_object:AddComponent("Animator")
    print("animator:" .. tostring(animator))
    animator:Awake()

    game_object1=animator:game_object()
    print("animator:game_object():" .. tostring(game_object1))

    print(game_object==game_object1)
    print(compare_lua_ref(game_object,game_object1))--这里不相等…… 传递到cpp那边看看为什么不相等，看LuaRef有没有相关接口判定。
    print(compare_game_object(game_object,game_object1))
end