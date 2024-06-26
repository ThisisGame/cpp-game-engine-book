---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by captain.
--- DateTime: 5/16/2022 10:55 PM
---
require("lua_extension")
require("cpp_class")

--- @class RenderTarget
RenderTarget=class("RenderTarget",CppClass)

function RenderTarget:ctor()
    RenderTarget.super.ctor(self)
end

--- 实例化C++ Class
function RenderTarget:InitCppClass()
    self.cpp_class_instance_=Cpp.RenderTarget()
end

--- 初始化RenderTarget，在GPU生成帧缓冲区对象(FrameBufferObject)
--- @param width number @宽
--- @param height number @高
function RenderTarget:Init(width,height)
    self.cpp_class_instance_:Init(width,height)
end

function RenderTarget:width()
    return self.cpp_class_instance_:width()
end

function RenderTarget:set_width(width)
    return self.cpp_class_instance_:set_width(width)
end

function RenderTarget:height()
    return self.cpp_class_instance_:height()
end

function RenderTarget:set_height(height)
    return self.cpp_class_instance_:set_height(height)
end

function RenderTarget:frame_buffer_object_handle()
    return self.cpp_class_instance_:frame_buffer_object_handle()
end

function RenderTarget:in_use()
    return self.cpp_class_instance_:in_use()
end

function RenderTarget:set_in_use(in_use)
    return self.cpp_class_instance_:set_in_use(in_use)
end