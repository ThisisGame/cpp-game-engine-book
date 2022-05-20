require("lua_extension")
require("component")
require("game_object_manager")
require("animator")
require("camera")

game_object=nil

function main()
    require("login_scene")

    game_object = GameObject.new()
    print("game_object:" .. tostring(game_object))

    print("----------- Add Animator --------------")

    local animator=game_object:AddComponent(Animator)
    print("add animator:" .. tostring(animator))

    print("----------- Add Camera ----------------")

    local camera=game_object:AddComponent(Camera)
    print("add camera:" .. tostring(camera))
end

function update()
    GameObjectManager:Update()
end