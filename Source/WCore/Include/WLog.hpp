#pragma once

#ifndef NDEBUG
    #include <iostream>
    #define WLOG(msg) std::cout << msg << std::endl
    #define WLOGFNAME(msg) std::cout << "[" << __func__ << "] " << msg << std::endl
#else
    #define WLOG(...)
    #define WLOGFNAME(...)
#endif
