

function main()
    require("login_scene")

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

    print("----------- Cpp.glm ------------")
    local position=glm.vec3(1,2,3)
    position.x=11
    position.y=22
    position.z=33
    print(tostring(position))
    camera:set_position(glm.vec3(4,5,6))
    print(camera:position())

    print("glm.vec3(4,5,6)+glm.vec3(4,5,6): " .. tostring(glm.vec3(4,5,6)+glm.vec3(4,5,6)))
    print("glm.vec3(4,5,6)-glm.vec3(4,5,6): " .. tostring(glm.vec3(4,5,6)-glm.vec3(4,5,6)))
    print("glm.vec3(4,5,6)*3: " .. tostring(glm.vec3(4,5,6)*3))
    print("glm.vec3(4,5,6)/3: " .. tostring(glm.vec3(4,5,6)/3))

    print("glm.radians(60): " .. glm.radians(60))
    print("glm.mat4(1.0)): " .. tostring(glm.mat4(1.0)))
    print("glm.vec4(1.0)): " .. tostring(glm.vec4(1.0,1.0,1.0,1.0)))
    print("glm.mat4(2.0) * glm.vec4(3.0,3.0,3.0,3.0)): " .. tostring(glm.mat4(2.0) * glm.vec4(3.0,3.0,3.0,3.0)))

    local rotate_mat4=glm.rotate(glm.mat4(1.0),glm.radians(60),glm.vec3(0.0,0.0,1.0))
    print("glm.rotate(glm.mat4(1.0),glm.radians(60),glm.vec3(0.0,0.0,1.0)): " .. tostring(rotate_mat4))
    print("rotate_mat4*glm.vec4(1,2,3,1.0): " .. tostring(rotate_mat4*glm.vec4(1,2,3,1.0)))

    print("-----------------------")




    game_object:test_set("hello")

    print(GetKeyActionUp())
    print(KeyAction.UP)
    print(GetKeyActionUp()==KeyAction.UP)

    print(GetKeyActionDown())
    print(KeyAction.DOWN)
    print(GetKeyActionDown()==KeyAction.DOWN)
end