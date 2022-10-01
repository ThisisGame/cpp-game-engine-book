//
// Created by captainchen on 2021/5/14.
//

#include "application_base.h"
#include <memory>
#include <iostream>
#include "rttr/registration"
#include "easy/profiler.h"
#include "utils/debug.h"
#include "component/game_object.h"
#include "renderer/camera.h"
#include "renderer/mesh_renderer.h"
#include "renderer/shader.h"
#include "control/input.h"
#include "utils/screen.h"
#include "render_device/render_task_consumer.h"
#include "audio/audio.h"
#include "utils/time.h"
#include "render_device/render_task_producer.h"
#include "physics/physics.h"


void ApplicationBase::Init() {
    EASY_MAIN_THREAD;
    profiler::startListen();// 启动profiler服务器，等待gui连接。

    Debug::Init();
    DEBUG_LOG_INFO("game start");
    Time::Init();

    //初始化图形库，例如glfw
    InitGraphicsLibraryFramework();

    UpdateScreenSize();

    //初始化 fmod
    Audio::Init();

    //初始化物理引擎
    Physics::Init();
}

/// 初始化图形库，例如glfw
void ApplicationBase::InitGraphicsLibraryFramework() {

}

void ApplicationBase::Run() {

}

void ApplicationBase::Update(){
    EASY_FUNCTION(profiler::colors::Magenta) // 标记函数
    Time::Update();
    UpdateScreenSize();

    GameObject::Foreach([](GameObject* game_object){
        if(game_object->active()){
            game_object->ForeachComponent([](Component* component){
                component->Update();
            });
        }
    });

    Input::Update();
    Audio::Update();
//    std::cout<<"ApplicationBase::Update"<<std::endl;
}


void ApplicationBase::Render(){
    EASY_FUNCTION(profiler::colors::Magenta); // 标记函数
    //遍历所有相机，每个相机的View Projection，都用来做一次渲染。
    Camera::Foreach([&](){
        GameObject::Foreach([](GameObject* game_object){
            if(game_object->active()==false){
                return;
            }
            MeshRenderer* mesh_renderer=game_object->GetComponent<MeshRenderer>();
            if(mesh_renderer== nullptr){
                return;
            }
            mesh_renderer->Render();
        });
    });
}

void ApplicationBase::FixedUpdate(){
    EASY_FUNCTION(profiler::colors::Magenta) // 标记函数
    Physics::FixedUpdate();

    GameObject::Foreach([](GameObject* game_object){
        if(game_object->active()){
            game_object->ForeachComponent([](Component* component){
                component->FixedUpdate();
            });
        }
    });
}

void ApplicationBase::OneFrame() {
    Update();
    // 如果一帧卡了很久，就多执行几次FixedUpdate
    float cost_time=Time::delta_time();
    while(cost_time>=Time::fixed_update_time()){
        FixedUpdate();
        cost_time-=Time::fixed_update_time();
    }

    Render();

    //发出特殊任务：渲染结束
    RenderTaskProducer::ProduceRenderTaskEndFrame();
}

void ApplicationBase::UpdateScreenSize() {
    RenderTaskProducer::ProduceRenderTaskUpdateScreenSize();
}

void ApplicationBase::Exit() {
    RenderTaskConsumer::Exit();
}