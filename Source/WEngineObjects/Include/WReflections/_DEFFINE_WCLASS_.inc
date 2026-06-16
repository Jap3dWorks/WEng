// -*- mode: C++; -*-

#ifndef WENG_CLASSNAME
#define WENG_CLASSNAME(_CLS) WENG_DCONCAT(WClass__,_CLS)
#endif

const WClass * _WCLASS_::StaticClass() noexcept {
    return & wrf::wclass::WENG_CLASSNAME_CLSS_VAR(_WCLASS_);
}

