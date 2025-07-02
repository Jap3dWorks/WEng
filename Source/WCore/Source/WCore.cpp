#include "WCore/WCore.hpp"
#include "WLog.h"
#include <cassert>

// WId
// ---
WId::WId(size_t id) : id_(id) {}

size_t WId::GetId() const
{
    return id_;
}

bool WId::IsValid() const
{
    return id_ > 0;
}

bool WId::operator==(const WId &other) const
{
    return id_ == other.id_;
}

bool WId::operator!=(const WId &other) const
{
    return id_ != other.id_;
}

bool WId::operator<(const WId &other) const
{
    return id_ < other.id_;
}

bool WId::operator>(const WId &other) const
{
    return id_ > other.id_;
}

bool WId::operator<=(const WId &other) const
{
    return id_ <= other.id_;
}

bool WId::operator>=(const WId &other) const
{
    return id_ >= other.id_;
}

WId& WId::operator++()
{
    ++id_;
    return *this;
}

WId WId::operator++(int)
{
    WId temp = WId(id_);
    ++id_;
    return temp;
}

WId& WId::operator--()
{
    --id_;
    return *this;
}

WId WId::operator--(int)
{
    WId temp = WId(id_);
    --id_;
    return temp;
}

WId& WId::operator+=(const WId &other)
{
    id_ += other.id_;
    return *this;
}

WId& WId::operator-=(const WId &other)
{
    id_ -= other.id_;
    return *this;
}

WId WId::operator+(const WId &other) const
{
    return WId(id_ + other.id_);
}

WId WId::operator-(const WId &other) const
{
    return WId(id_ - other.id_);
}

WId::operator size_t() const
{
    return id_;
}

// WClass
// ------

WClass::WClass(const char *name) : name_(name)
{
}

const char* WClass::GetName() const
{
    return name_;
}

bool WClass::operator==(const WClass &other) const
{
    return name_ == other.name_;
}

bool WClass::operator!=(const WClass &other) const
{
    return name_ != other.name_;
}

bool WClass::operator<(const WClass &other) const
{
    return name_ < other.name_;
}

bool WClass::operator>(const WClass &other) const
{
    return name_ > other.name_;
}

bool WClass::operator<=(const WClass &other) const
{
    return name_ <= other.name_;
}

bool WClass::operator>=(const WClass &other) const
{
    return name_ >= other.name_;
}

// WObject
// -------

void WObject::WID(WId in_wid)
{
    assert(!wid_.IsValid() && "Object already has a valid ID!");
    
    wid_ = in_wid;
}
