#define GLFW_INCLUDE_NONE

#include <easy/profiler.h>
#include "component/game_object.h"
#include "utils/application.h"

int main(void){
    EASY_MAIN_THREAD;
    profiler::startListen();// 启动profiler服务器，等待gui连接。

    Application::set_title("[loadbank] press s play event,press 1 2 3 set param");
    Application::set_data_path("../data/");//设置资源目录

    Application::Init();//初始化引擎

    GameObject* go=new GameObject("LoginSceneGo");
    go->AddComponent("Transform");
    go->AddComponent("LoginScene");

    Application::Run();//开始引擎主循环
}