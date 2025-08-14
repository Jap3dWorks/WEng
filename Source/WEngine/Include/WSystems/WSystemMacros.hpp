#pragma once

#define DECLARE_LEVEL_WSYSTEM(_FN) bool _FN(WLevel * in_level, WEngine * in_engine); \
    void _FN ## _REG(WSystemRegistry & in_registry);

#define DECLARE_WSYSTEM(_FN) bool _FN(WEngine * in_engine); \
    void _FN ## _REG(WSystemRegistry & in_registry);

#define START_DEFINE_LEVEL_WSYSTEM(_FN) bool WSystem:: _FN(WLevel * in_level, WEngine * in_engine) { \

#define END_DEFINE_LEVEL_WSYSTEM(_FN) } \
        void WSystem:: _FN ## _REG(WSystemRegistry & in_registry) {     \
        in_registry.Registry(#_FN, _FN);                                \
    }
