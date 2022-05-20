//
// Created by captainchen on 2022/2/7.
//

#ifndef UNTITLED_RENDERER_H
#define UNTITLED_RENDERER_H

#include <thread>
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Renderer {
public:
    Renderer(GLFWwindow* window);
    ~Renderer();

private:
    /// 渲染线程主函数
    void RenderMain();
private:
    GLFWwindow* window_;//glfw创建的窗口
    std::thread render_thread_;//渲染线程
};


#endif //UNTITLED_RENDERER_H
