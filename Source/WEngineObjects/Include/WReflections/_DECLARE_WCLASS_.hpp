#include "WReflections/WClassFor.hpp"

#ifndef _WCLASS_
#define _WCLASS_ WObject
#endif

#ifndef _PWCLASS_
#define _PWCLASS_ WObject
#endif

#define DCONCAT(A,B) A##B

#ifdef _CLASSNAME
#undef _CLASSNAME
#endif

#define _CLASSNAME(_CLS) DCONCAT(WClass__,_CLS)

#define _XSTR(x) #x
#define _STR(x) _XSTR(x)

#define _MP(x) x

#define _CLASSNAME_STR(_CLS) _STR(_CLS)

#define _CLASSNAME_VAR(_CLS) DCONCAT(_NAME_, _CLS)

class _WCLASS_;
class _PWCLASS_;

constexpr const char _CLASSNAME_VAR(_MP( _WCLASS_ )) [WCLASSFOR_N_SIZE] = _CLASSNAME_STR(_WCLASS_);

using _CLASSNAME(_WCLASS_) = WClassFor<_WCLASS_, _CLASSNAME_VAR(_MP(_WCLASS_)), _PWCLASS_>;


