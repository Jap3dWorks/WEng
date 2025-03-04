#pragma once

#include "DllDef.h"
#include "WLibraryLink.h"
#include "WLog.h"
#include "WCore/CoreMacros.h"

#include <string>
#include <functional>
#include <cstdint>

#include "WCore.intermediate.h"

namespace detail
{
    class WObjectContainerBase;
}

/**
 * @brief Basic identifier token
*/
class WCORE_API WId
{
public:

    WId() = default;
    WId(size_t id);

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

    WId& operator++();
    WId operator++(int);

    WId& operator--();
    WId operator--(int);

    WId& operator+=(const WId &other);
    WId& operator-=(const WId &other);

    size_t id_=0; 

    friend detail::WObjectContainerBase;
};

namespace std
{
    template <>
    struct hash<WId>
    {
        std::size_t operator()(const WId & in_wid) const
        {
            return std::hash<std::size_t>{}(in_wid.GetId());
        }
    };
}


class WCORE_API WClass
{
public:

    WClass() = delete;
    WClass(const char *name);

    virtual ~WClass() = default;

    const char *GetName() const;

public:
    bool operator==(const WClass &other) const;
    bool operator!=(const WClass &other) const;

    bool operator<(const WClass &other) const;
    bool operator>(const WClass &other) const;

    bool operator<=(const WClass &other) const;
    bool operator>=(const WClass &other) const;

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

