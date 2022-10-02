//
// Created by captainchen on 2021/5/14.
//

#ifndef UNTITLED_APPLICATION_BASE_H
#define UNTITLED_APPLICATION_BASE_H

#include <string>

class ApplicationBase {
public:
    ApplicationBase(){}
    virtual ~ApplicationBase(){}

    void set_title(std::string title){title_=title;}

    const std::string& data_path(){return data_path_;}
    void set_data_path(std::string data_path){data_path_=data_path;}

    /// 初始化OpenGL
    virtual void Init();

    /// 初始化图形库，例如glfw
    virtual void InitGraphicsLibraryFramework();

    ///
    virtual void InitLuaBinding();

    virtual void Run();

    /// 一帧
    virtual void OneFrame();

    virtual void UpdateScreenSize();

    /// 每一帧内逻辑代码。
    virtual void Update();

    virtual void FixedUpdate();

    /// 逻辑代码执行后，应用到渲染。
    virtual void Render();

    virtual void Exit();

protected:
    std::string title_;//标题栏显示

    std::string data_path_;//资源目录
};


#endif //UNTITLED_APPLICATION_BASE_H
