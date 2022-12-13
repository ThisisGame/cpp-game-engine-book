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

    static const std::string& data_path();

    static void Run();
private:
    static ApplicationBase* instance_;
};


#endif //UNTITLED_APPLICATION_H
