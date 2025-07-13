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

