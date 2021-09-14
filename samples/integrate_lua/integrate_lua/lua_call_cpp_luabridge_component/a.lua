print("-------run lua--------")

LoginScene={}
function LoginScene:Awake()
    print("LoginScene Awake")
end

function LoginScene:Update()
    print("LoginScene Update")
end

function main()
    local go = GameObject()
    print("go:" .. tostring(go))

    --local animator=Animator()
    --print("animator:" .. tostring(animator))
    --
    --animator:Awake()
    --animator:set_game_object(go)
    --print("animator:game_object():" .. tostring(animator:game_object()))

    print("---------------------------")

    animator=go:AddComponent("Animator")
    print("animator:" .. tostring(animator))
    animator:Awake()

    local game_object=animator:game_object()
    print("animator:game_object():" .. tostring(game_object))

    print("---------------------------")

    local camera=game_object:AddComponent("Camera")
    print("camera:" .. tostring(camera))
    camera:Awake()

    game_object=camera:game_object()
    print("camera:game_object():" .. tostring(game_object))

    --local loginScene=go:AddComponent(LoginScene)
    --print(loginScene)
end
