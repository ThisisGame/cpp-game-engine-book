//
// Created by captainchen on 2021/8/23.
//

#include "debug.h"
#include <iostream>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

void Debug::Init() {
    try
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
//        console_sink->set_pattern("[source %g] [function %!] [line %#] [%^%l%$] %v");

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
        file_sink->set_level(spdlog::level::trace);

        // you can even set multi_sink logger as default logger
        spdlog::set_default_logger(std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({console_sink, file_sink})));
        spdlog::set_pattern("[source %s] [function %!] [line %#] [%^%l%$] %v");
        spdlog::flush_on(spdlog::level::trace);// flush on every log call.

        DEBUG_LOG_INFO("spdlog init success");
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}
