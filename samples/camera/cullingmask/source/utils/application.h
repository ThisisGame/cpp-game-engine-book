//
// Created by captainchen on 2021/5/14.
//

#ifndef UNTITLED_APPLICATION_H
#define UNTITLED_APPLICATION_H

#include <string>

class Application {
public:
    static const std::string& data_path(){return data_path_;}
    static void set_data_path(std::string data_path){data_path_=data_path;}
private:
    static std::string data_path_;
};


#endif //UNTITLED_APPLICATION_H
