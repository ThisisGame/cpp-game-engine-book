#define GLFW_INCLUDE_NONE

#include <easy/profiler.h>
#include "component/game_object.h"
#include "utils/application.h"

int main(void){
    EASY_MAIN_THREAD;
    EASY_PROFILER_ENABLE;
    profiler::startListen();// 启动profiler服务器，等待gui连接。
    EASY_FUNCTION(profiler::colors::Magenta); // 标记函数

    EASY_BLOCK("set_title set_data_path"); // 标记代码块 使用默认颜色 Amber100
    {
        Application::set_title("[loadbank] press s play event,press 1 2 3 set param");
        Application::set_data_path("../data/");//设置资源目录
    }
    EASY_END_BLOCK; // 结束代码块

    Application::Init();//初始化引擎


    EASY_BLOCK("create loginscene go", profiler::colors::Blue500); // 标记代码块 使用颜色 Blue500
    {
        GameObject* go=new GameObject("LoginSceneGo");
        go->AddComponent("Transform");
        go->AddComponent("LoginScene");
    }
    EASY_END_BLOCK; // 结束代码块

    Application::Run();//开始引擎主循环

}