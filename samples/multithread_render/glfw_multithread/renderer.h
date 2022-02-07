//
// Created by captainchen on 2022/2/7.
//

#ifndef UNTITLED_RENDERER_H
#define UNTITLED_RENDERER_H

#include <thread>

class GLFWwindow;
class Renderer {
public:
    Renderer(GLFWwindow* window);
    ~Renderer();

private:
    void RenderMain();
private:
    GLFWwindow* window_;
    std::thread render_thread_;
};


#endif //UNTITLED_RENDERER_H
