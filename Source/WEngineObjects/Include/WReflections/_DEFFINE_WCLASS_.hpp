
WENG_CLASSNAME(_WCLASS_) & TSingleton<WClass, WENG_CLASSNAME(_WCLASS_)>::Value() {
    static WENG_CLASSNAME(_WCLASS_) _v{};
    return _v;
}

const WClass * _WCLASS_::StaticClass() noexcept {
    return &TSingleton<WClass, WENG_CLASSNAME(_WCLASS_)>::Value();
}
