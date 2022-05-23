require("lua_extension")
require("component")
require("game_object_manager")
require("animator")
require("camera")
require("ui_camera")
require("login_scene")

game_object=nil

function main()
    game_object = GameObject.new()
    print("game_object:" .. tostring(game_object))

    print("----------- Add Animator --------------")

    local animator=game_object:AddComponent(Animator)
    print("add animator:" .. tostring(animator))

    print(animator:game_object())

    print("----------- Add Camera ----------------")

    local camera=game_object:AddComponent(Camera)
    print("add camera:" .. tostring(camera))

    print("----------- Add UICamera ----------------")

    local ui_camera=game_object:AddComponent(UICamera)
    print("add ui_camera:" .. tostring(ui_camera))

    print("----------- Add LoginScene ------------")

    local login_scene=game_object:AddComponent(LoginScene)
    print("add LoginScene:" .. tostring(login_scene))

    print("----------- Get LoginScene ------------")
    login_scene=game_object:GetComponent(LoginScene)
    print("get LoginScene:" .. tostring(login_scene))

    print("----------- Get Camera ------------")
    camera=login_scene:game_object():GetComponent(Camera)
    print("get camera:" .. tostring(camera))

    print("----------- Get Animator ------------")
    if camera==nil then
        print("camera==nil")
    end
    if camera:game_object()==nil then
        print("camera:game_object()==nil")
    end
    animator=camera:game_object():GetComponent(Animator)
    print("get animator:" .. tostring(animator))
end

function update()
    GameObjectManager:Update()
end