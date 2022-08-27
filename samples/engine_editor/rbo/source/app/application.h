//
// Created by captainchen on 2021/5/14.
//

#ifndef UNTITLED_APPLICATION_H
#define UNTITLED_APPLICATION_H

#include <string>


class GLFWwindow;
class RenderTaskConsumer;
class Application {
public:
    static void set_title(std::string title){title_=title;}

    static const std::string& data_path(){return data_path_;}
    static void set_data_path(std::string data_path){data_path_=data_path;}

    /// 初始化OpenGL
    static void Init(unsigned short window_width,unsigned short window_height);

    static void Run();

    /// 每一帧内逻辑代码。
    static void Update();

    static void FixedUpdate();

    /// 逻辑代码执行后，应用到渲染。
    static void Render();

private:
    static std::string title_;//标题栏显示

    static std::string data_path_;//资源目录

    static GLFWwindow* glfw_window_;

};


#endif //UNTITLED_APPLICATION_H
