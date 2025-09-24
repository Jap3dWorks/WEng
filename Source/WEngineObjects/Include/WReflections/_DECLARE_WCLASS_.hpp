#include "WCore/WCore.hpp"
#include "WCore/TSingleton.hpp"
#include "WReflections/WClassFor.hpp"

#ifndef _WCLASS_
#define _WCLASS_ WObject
#endif

#ifndef _PWCLASS_
#define _PWCLASS_ WObject
#endif

#ifndef WENG_DCONCAT
#define WENG_DCONCAT(A,B) A##B
#endif

#ifndef WENG_CLASSNAME
#define WENG_CLASSNAME(_CLS) WENG_DCONCAT(WClass__,_CLS)
#endif

#ifndef WENG_MP
#define WENG_MP(x) x
#endif

#ifndef WENG_CLASSNAME_STR
#define WENG_CLASSNAME_STR(_CLS) _STR(_CLS)
#endif

#ifndef WENG_CLASSNAME_VAR
#define WENG_CLASSNAME_VAR(_CLS) WENG_DCONCAT(_NAME_, _CLS)
#endif

class _WCLASS_;
class _PWCLASS_;

constexpr const char WENG_CLASSNAME_VAR(WENG_MP( _WCLASS_ )) [WCLASSFOR_N_SIZE] = WENG_CLASSNAME_STR(_WCLASS_);

using WENG_CLASSNAME(_WCLASS_) = WClassFor<_WCLASS_, WENG_CLASSNAME_VAR(WENG_MP(_WCLASS_)), _PWCLASS_>;

// template<>
// struct TSingleton<WClass, WENG_CLASSNAME(_WCLASS_)> {
//     _WENG_API_ static WENG_CLASSNAME(_WCLASS_) & Value();
// };

