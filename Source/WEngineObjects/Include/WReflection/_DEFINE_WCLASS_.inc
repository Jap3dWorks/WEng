// -*- mode: C++ -*-
// [[file:../../WEngineObjects.org::define_wclass-inc][define_wclass-inc]]
#ifndef WENG_CLASSNAME
#define WENG_CLASSNAME(_CLS) WENG_DCONCAT(WClass__, _CLS)
#endif

#ifndef WENG_CLASSNAME_CLSS_VAR
#define WENG_CLASSNAME_CLSS_VAR(_CLS) WENG_DCONCAT(_VAR_, _CLS)
#endif

#ifndef WENG_CLASSNAME_STR
#define WENG_CLASSNAME_STR(_CLS) _STR(_CLS)
#endif

#ifndef WENG_CLASSNAME_STR_VAR
#define WENG_CLASSNAME_STR_VAR(_CLS) WENG_DCONCAT(_NAME_, _CLS)
#endif

const WClass * _WCLASS_NAMESPACE_ :: _WCLASS_::StaticClass() noexcept {
    return & wrf::wclass::WENG_CLASSNAME_CLSS_VAR(_WCLASS_);
}
// define_wclass-inc ends here
