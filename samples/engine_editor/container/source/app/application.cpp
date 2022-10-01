//
// Created by captainchen on 2021/5/14.
//

#include "application.h"
#include "application_base.h"

ApplicationBase* Application::instance_= nullptr;
std::string Application::title_;
std::string Application::data_path_;

void Application::Init(ApplicationBase* instance) {
    instance_=instance;
    instance_->set_title(title_);
    instance_->set_data_path(data_path_);
    instance_->Init();
}

void Application::set_title(std::string title) {
    title_ = title;
}

const std::string& Application::data_path() {
    return data_path_;
}

void Application::set_data_path(std::string data_path) {
    data_path_ = data_path;
}

void Application::Run() {
    instance_->Run();
}