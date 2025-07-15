#include "WReflections/WClassFor.hpp"

#ifndef _WCLASS_
#define _WCLASS_ WObject
#endif

#define DCONCAT(A,B) A##B

#ifdef _CLASSNAME
#undef _CLASSNAME
#endif

#define _CLASSNAME(_CLS) DCONCAT(WClass__,_CLS)

class _WCLASS_;

using _CLASSNAME(_WCLASS_) = WClassFor<_WCLASS_>;


