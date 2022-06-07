﻿---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by captain.
--- DateTime: 5/16/2022 10:55 PM
---

require("lua_extension")

--- @class UIImage @图片
UIImage=class("UIImage",Component)

function UIImage:ctor()
    UIImage.super.ctor(self)
    ---@field texture_2d_ Texture2D
    self.texture_2d_=nil
end

function UIImage:InitCppComponent()
    ---@type Cpp.UIImage
    self.cpp_component_instance_=Cpp.UIImage() --创建对应的C++实例
end

--- 设置Texture2D
--- @param texture_2d Texture2D
function UIImage:set_texture(texture_2d)
    self.texture_2d_=texture_2d
    self.cpp_component_instance_:set_texture(texture_2d:cpp_class_instance())
end