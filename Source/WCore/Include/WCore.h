#pragma once
#include "DllDef.h"
#include <cstdint>
#include "WLog.h"
#include <string>
#include <functional>
#include "WCore/CoreMacros.h"

#include "WCore.intermediate.h"


class WCORE_API WId
{
private:
    size_t id_; 
};


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

WCLASS()
class WCORE_API WObject
{
    WOBJECT_BODY(WObject)

public:

private:
    // static const WObject default_object_{}; 

protected:
    WId id_{};
};

