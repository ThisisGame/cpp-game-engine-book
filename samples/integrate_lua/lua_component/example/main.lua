function main()
    Cpp.Application.set_title("[loadbank] press s play event,press 1 2 3 set param")
    Cpp.Application.set_data_path("../data/")--设置资源目录
    Cpp.Application.Init()--初始化引擎

    local go=Cpp.GameObject("LoginSceneGo")
    go:AddComponent("Transform")
    go:AddComponent("LoginScene")

    Cpp.Application.Run()--开始引擎主循环
end