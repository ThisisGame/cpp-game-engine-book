function main()
    Cpp.Application.set_title("[loadbank] press s play event,press 1 2 3 set param")
    Cpp.Application.set_data_path("../data/")--设置资源目录
    Cpp.Application.Init()--初始化引擎

    local go=Cpp.GameObject("LoginSceneGo")
    local transform=go:AddComponent("Transform")
    print("transform:" .. tostring(transform))
    local pos=transform:position()
    print("pos:" .. tostring(pos))
    --go:AddComponent(LoginScene)

    Cpp.Application.Run()--开始引擎主循环
end