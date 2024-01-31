#pragma once
#include <iostream>

#ifndef NDEBUG
    #define WLOG(msg) std::cout << msg << std::endl;
#else
    #define WLOG(...)
#endif
