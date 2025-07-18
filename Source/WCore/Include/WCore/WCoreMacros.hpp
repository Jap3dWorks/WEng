#pragma once

#define WNODISCARD [[nodiscard]]

#define WINLINE inline

#define WCNOEXCEPT const noexcept

#define WNOEXCEPT noexcept

#ifndef NDEBUG
#define WDEBUGBUILD 1
#else
#define WDEBUGBUILD 0
#endif

#ifndef _STR
#define _XSTR(x) #x
#define _STR(x) _XSTR(x)
#endif
