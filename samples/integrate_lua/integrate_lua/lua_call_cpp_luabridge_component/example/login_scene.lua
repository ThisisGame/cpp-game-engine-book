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