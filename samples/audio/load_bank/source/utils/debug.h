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
    static void Log(const T &msg);

    template<typename T>
    static void LogError(const T &msg);

    template<typename T>
    static void LogWarning(const T &msg);

    template<typename FormatString, typename... Args>
    static void Log(const FormatString &fmt, Args&&...args);

    template<typename FormatString, typename... Args>
    static void LogError(const FormatString &fmt, Args&&...args);

    template<typename FormatString, typename... Args>
    static void LogWarning(const FormatString &fmt, Args&&...args);
};


#endif //UNTITLED_DEBUG_H
