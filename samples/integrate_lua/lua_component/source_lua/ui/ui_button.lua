﻿---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by captain.
--- DateTime: 5/16/2022 10:55 PM
---

require("lua_extension")
require("cpp_component")

UIButton=class("UIButton",CppComponent)

function UIButton:ctor()
    UIButton.super.ctor(self)
end

function UIButton:InitCppComponent()
    ---@type Cpp.UIButton
    self.cpp_component_instance_=Cpp.UIButton() --创建对应的C++实例
end

function UIButton:set_image_normal(image)
    self.cpp_component_instance_:set_image_normal()
end