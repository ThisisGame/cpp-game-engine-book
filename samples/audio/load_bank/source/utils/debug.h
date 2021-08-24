//
// Created by captainchen on 2021/8/23.
//

#ifndef UNTITLED_DEBUG_H
#define UNTITLED_DEBUG_H
#include "spdlog/spdlog.h"

class Debug {
public:
    static void Init();

    template<typename T>
    static void Log(const T &msg){
        spdlog::info(msg);
    };

    template<typename T>
    static void LogError(const T &msg){
        spdlog::error(msg);
    };

    template<typename T>
    static void LogWarning(const T &msg){
        spdlog::warn(msg);
    };

    template<typename FormatString, typename... Args>
    static void Log(const FormatString &fmt, Args&& ...args){
        spdlog::info(fmt,std::forward<Args>(args)...);
    };

    template<typename FormatString, typename... Args>
    static void LogError(const FormatString &fmt, Args&&...args){
        spdlog::error(fmt,std::forward<Args>(args)...);
    };

    template<typename FormatString, typename... Args>
    static void LogWarning(const FormatString &fmt, Args&&...args){
        spdlog::warn(fmt,std::forward<Args>(args)...);
    };
};


#endif //UNTITLED_DEBUG_H
