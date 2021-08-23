//
// Created by captainchen on 2021/8/23.
//

#include "debug.h"
#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"


void Debug::Init() {
    try
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("[engine] [%^%l%$] %v");

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
        file_sink->set_level(spdlog::level::trace);

        spdlog::sinks_init_list sink_list = { file_sink, console_sink };

        // you can even set multi_sink logger as default logger
        spdlog::set_default_logger(std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({console_sink, file_sink})));
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}

template<typename T>
void Debug::Log(const T &msg){
    spdlog::info(msg);
}

template<typename T>
void Debug::LogError(const T &msg) {
    spdlog::error(msg);
}

template<typename T>
void Debug::LogWarning(const T &msg) {
    spdlog::warn(msg);
}

template<typename FormatString, typename... Args>
void Debug::Log(const FormatString &fmt, Args &&... args) {
    spdlog::log(fmt,std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
void Debug::LogError(const FormatString &fmt, Args &&... args) {
    spdlog::error(fmt,std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
void Debug::LogWarning(const FormatString &fmt, Args &&... args) {
    spdlog::warn(fmt,std::forward<Args>(args)...);
}
