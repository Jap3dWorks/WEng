#pragma once

#include "DllDef.h"
#include "WCore/CoreMacros.hpp"

#include <string>
#include <functional>

#include "WCore.intermediate.h"


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

    bool IsValid() const;

private:

    WId& operator++();
    WId operator++(int);

    WId& operator--();
    WId operator--(int);

    WId& operator+=(const WId &other);
    WId& operator-=(const WId &other);

    size_t id_=0; 
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
    const char * name_;
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

    WNODISCARD WId WID() const { return wid_; }

    void WID(WId in_wid);

    

protected:

    WId wid_{};

private:

    // static const WObject default_object_{}; 
};

