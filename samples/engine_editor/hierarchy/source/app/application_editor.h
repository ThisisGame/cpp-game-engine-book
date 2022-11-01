//
// Created by captainchen on 2021/5/14.
//

#ifndef UNTITLED_APPLICATION_EDITOR_H
#define UNTITLED_APPLICATION_EDITOR_H

#include <string>
#include "application_base.h"
#include "data_structs/tree.h"

struct GLFWwindow;
class GameObject;
class ApplicationEditor : public ApplicationBase{
public:
    ApplicationEditor():ApplicationBase(){}
    ~ApplicationEditor(){}

    void Run();

    void DrawHierarchy(Tree::Node* node,const char* label,int base_flags);
public:
    /// 初始化图形库，例如glfw
    virtual void InitGraphicsLibraryFramework() override;

    virtual void Exit() override;
private:
    GLFWwindow* editor_glfw_window_;//编辑器窗口
    GLFWwindow* game_glfw_window_;//游戏窗口
};


#endif //UNTITLED_APPLICATION_EDITOR_H
