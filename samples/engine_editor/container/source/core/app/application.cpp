//
// Created by captainchen on 2021/5/14.
//

#include "application.h"
#include "application_base.h"

ApplicationBase* Application::instance_= nullptr;

void Application::Init(ApplicationBase* instance) {
    instance_=instance;
    instance_->Init();
}

const std::string& Application::data_path() {
    return instance_->data_path();
}

void Application::Run() {
    instance_->Run();
}