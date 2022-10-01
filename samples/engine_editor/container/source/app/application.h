//
// Created by captainchen on 2021/5/14.
//

#ifndef UNTITLED_APPLICATION_H
#define UNTITLED_APPLICATION_H

#include <string>

class ApplicationBase;
class Application {
public:
    static void Init(ApplicationBase* instance);

    static void set_title(std::string title);

    static const std::string& data_path();

    static void set_data_path(std::string data_path);

    static void Run();
private:
    static ApplicationBase* instance_;

    static std::string title_;//标题栏显示

    static std::string data_path_;//资源目录
};


#endif //UNTITLED_APPLICATION_H
