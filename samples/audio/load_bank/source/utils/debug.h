//
// Created by captainchen on 2021/8/23.
//

#ifndef UNTITLED_DEBUG_H
#define UNTITLED_DEBUG_H

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"

/// 输出文件名
#define DEBUG_LOG_INFO SPDLOG_INFO
#define DEBUG_LOG_WARN SPDLOG_WARN
#define DEBUG_LOG_ERROR SPDLOG_ERROR

//#define DEBUG_LOG_INFO(msg, ...) SPDLOG_INFO(msg,__FILE__, __LINE__,SPDLOG_FUNCTION,__VA_ARGS__)
//#define DEBUG_LOG_INFO(msg, ...) spdlog::log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},spdlog::level::info,__VA_ARGS__)

class Debug {
public:
    static void Init();
};


#endif //UNTITLED_DEBUG_H
