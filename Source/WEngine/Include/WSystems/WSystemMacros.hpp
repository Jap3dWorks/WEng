#pragma once

#include "WCore/WCore.hpp"

class WSystemsRegister;
class WLevel;
class WEngine;

#define START_WSYSTEMS_REG(_MODULE, _NAME) namespace WSystems {   \
    void _MODULE ## _ ## _NAME ## _REG(WSystemsRegister&);

#define END_WSYSTEMS_REG() }

#define DECLARE_LEVEL_WSYSTEM(_FN) bool _FN(WLevel * in_level, WEngine * in_engine); \
    void _FN ## _REG(WSystemsRegister & in_registry);

#define DECLARE_WSYSTEM(_FN) bool _FN(WEngine * in_engine); \
    void _FN ## _REG(WSystemsRegister & in_registry);

#define START_DEFINE_LEVEL_WSYSTEM(_FN)                                 \
    void WSystems:: _FN ## _REG(WSystemsRegister & in_registry) {        \
        in_registry.RegLevelSystem(#_FN, _FN);                          \
    }                                                                   \
    bool WSystems:: _FN(WLevel * in_level, WEngine * in_engine) {

#define END_DEFINE_LEVEL_WSYSTEM() }

#define START_DEFINE_WSYSTEM(_FN) \
    void WSystems:: _FN ## _REG(WSystemsRegister & in_register) {    \
        in_register.RegSystem(#_FN, _FN)                            \
            }                                                       \
    bool SYstems:: _FN(WEngine * in_engine) {

#define END_DEFINE_WSYSTEM() }

#define START_DEFINE_WSYSTEMS_REG(_MODULE, _NAME) void WSystems:: _MODULE ## _ ## _NAME ## _REG (WSystemsRegister & in_register) {

#define END_DEFINE_WSYSTEMS_REG() }

#define CALL_WSYSTEM_REGISTER(_FN) _FN ## _REG(in_register);

#define REG_WSYSTEMS(_MODULE, _NAME, in_register) WSystems:: _MODULE ## _ ## _NAME ## _REG(in_register);
