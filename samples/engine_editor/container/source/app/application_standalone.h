//
// Created by captainchen on 2021/5/14.
//

#ifndef UNTITLED_APPLICATION_STANDALONE_H
#define UNTITLED_APPLICATION_STANDALONE_H

#include <string>
#include "application_base.h"

class GLFWwindow;
class ApplicationStandalone : public ApplicationBase{
public:
    ApplicationStandalone():ApplicationBase(){}
    ~ApplicationStandalone(){}

    void Run();

public:
    /// 初始化图形库，例如glfw
    virtual void InitGraphicsLibraryFramework() override;

    virtual void Exit() override;
private:
    GLFWwindow* glfw_window_;
};


#endif //UNTITLED_APPLICATION_STANDALONE_H
