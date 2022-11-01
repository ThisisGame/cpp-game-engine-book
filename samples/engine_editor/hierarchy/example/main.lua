require("utils/debug")
require("utils/application")
require("component/game_object")
require("component/transform")
require("login_scene")

function main()
    Debug.ConnectDebugServer()

    local go=GameObject.new("LoginSceneGo")
    local transform=go:AddComponent(Transform)
    print("transform:" .. tostring(transform))
    local pos=transform:position()
    print("pos:" .. tostring(pos))

    go:AddComponent(LoginScene)
end