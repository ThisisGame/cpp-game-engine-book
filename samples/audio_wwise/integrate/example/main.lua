function main()
    require("game_scene")

    Application.set_title("Hunter")
    Application.set_data_path("../data/")--设置资源目录
    Application.Init()--初始化引擎

    local go=GameObject("LoginSceneGo")
    local transform=go:AddComponent("Transform")
    print("transform:" .. tostring(transform))
    local pos=transform:position()
    print("pos:" .. tostring(pos))

    go:AddComponent("GameScene")

    Application.Run()--开始引擎主循环
end