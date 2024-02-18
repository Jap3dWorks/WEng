#pragma once
#include "DllDef.h"
#include <cstdint>
#include "WLog.h"
#include <string>
#include <functional>
#include "WCore/CoreMacros.h"

#include "WCore.intermediate.h"

namespace detail
{
    class WObjectContainerBase;
}


class WCORE_API WId
{
public:
    WId() = default;
    size_t GetId() const;

    bool operator==(const WId &other) const;
    bool operator!=(const WId &other) const;

    bool operator<(const WId &other) const;
    bool operator>(const WId &other) const;

    bool operator<=(const WId &other) const;
    bool operator>=(const WId &other) const;

    WId operator+(const WId &other) const;
    WId operator-(const WId &other) const;

    operator size_t() const;

private:
    WId(size_t id);
    WId& operator++();
    WId operator++(int);

    WId& operator--();
    WId operator--(int);

    WId& operator+=(const WId &other);
    WId& operator-=(const WId &other);

    size_t id_=0; 

    friend detail::WObjectContainerBase;
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

protected:
    WId id_{};

private:
    // static const WObject default_object_{}; 

friend detail::WObjectContainerBase;

};

