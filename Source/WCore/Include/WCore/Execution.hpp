#pragma once

#define WCORE_STATIC_EXECUTE_ONCE(X)            \
    static bool _wcr_exec_executed_=false;      \
    if (!_wcr_exec_executed_) {                 \
        X;                                      \
        _wcr_exec_executed_ = true;             \
    }                                                             
