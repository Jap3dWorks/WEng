// -*- mode: C++ -*-
// [[file:../../../../org/WEngineObjects.org::DECLARE_WCLASS][DECLARE_WCLASS]]
#include "WCore/WCore.hpp"
#include "WCore/TSingleton.hpp"
#include "WReflection/WClassFor.hpp"

#ifndef _WCLASS_
#define _WCLASS_ WObject
#endif

#ifndef _PWCLASS_
#define _PWCLASS_ WObject
#endif

#ifndef _WCLASS_DECLARATION_
#define _WCLASS_DECLARATION_ class _WCLASS_;
#endif

#ifndef _PWCLASS_DECLARATION_
#define _PWCLASS_DECLARATION_ class _PWCLASS_;
#endif

#ifndef WENG_DCONCAT
#define WENG_DCONCAT(A,B) A##B
#endif

#ifndef WENG_MP
#define WENG_MP(x) x
#endif

#ifndef WENG_CLASSNAME
#define WENG_CLASSNAME(_CLS) WENG_DCONCAT(WClass__,_CLS)
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

_WCLASS_DECLARATION_
_PWCLASS_DECLARATION_

namespace wrf::wclass {

    inline constexpr std::string_view WENG_CLASSNAME_STR_VAR(WENG_MP( _WCLASS_ )) =
        WENG_CLASSNAME_STR(_WCLASS_);

    using WENG_CLASSNAME(_WCLASS_) = WClassFor<_WCLASS_, _PWCLASS_>;

    inline constexpr WENG_CLASSNAME(_WCLASS_) WENG_CLASSNAME_CLSS_VAR(_WCLASS_) =
        WENG_CLASSNAME_STR_VAR(WENG_MP( _WCLASS_ )) ;
}
// DECLARE_WCLASS ends here
