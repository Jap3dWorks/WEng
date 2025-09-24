#ifndef WENG_CLASSNAME
#define WENG_CLASSNAME(_CLS) WENG_DCONCAT(WClass__,_CLS)
#endif

// WENG_CLASSNAME(_WCLASS_) & TSingleton<WClass, WENG_CLASSNAME(_WCLASS_)>::Value() {
//     static WENG_CLASSNAME(_WCLASS_) _v{};
//     return _v;
// }

const WClass * _WCLASS_::StaticClass() noexcept {
    static WENG_CLASSNAME(_WCLASS_) _v{};
    return &_v;
    // return &TSingleton<WClass, WENG_CLASSNAME(_WCLASS_)>::Value();
}
