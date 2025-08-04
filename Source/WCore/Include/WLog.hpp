#pragma once

#ifndef NDEBUG
    #include <print>

    #define WLOG(msg, ...) std::print("[WEng] " msg "\n" ,##__VA_ARGS__ );
    #define WFLOG(msg, ...) std::print("[WEng][{}] {}\n", __func__, std::format(msg ,##__VA_ARGS__));
    #define WFLOG_Warning(msg, ...) std::print("[WEng][WARNING][{}] {}\n", __func__, std::format(msg, ##__VA_ARGS__));
 

#else
    #define WLOG(...)
    #define WFLOG(...)
    #define WFLOG_Warning(...)
#endif
