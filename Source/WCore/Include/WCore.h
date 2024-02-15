#pragma once
#include "DllDef.h"
#include <cstdint>
#include "WLog.h"
#include <string>
#include <functional>

typedef uint32_t WId;

class WCORE_API WClass
{
public:
    WClass() = delete;
    WClass(const char *name);

    virtual ~WClass() = default;

    const char *GetName() const;

private:
    const char *name_;
};

namespace std
{
    template <>
    struct hash<WClass>
    {
        std::size_t operator()(const WClass &wclass) const
        {
            return std::hash<std::string>{}(wclass.GetName());
        }

    };
}

class WCORE_API WObject
{
protected:
    WId id_{};

public:
    virtual WClass GetClass() const = 0;
};

