#pragma once

#include "WCore/WCore.hpp"

class WENGINEOBJECTS_API WClass
{
public:

    constexpr WClass() noexcept = default;
    constexpr WClass(const char *name) noexcept :
    name_(name) {}

    virtual ~WClass() = default;

    constexpr const char *GetName() const noexcept {
        return name_;
    }

public:

    constexpr bool operator==(const WClass &other) const noexcept
    {
        return std::string_view(name_) == other.name_;
    }
    
    constexpr bool operator!=(const WClass &other) const noexcept
    {
        return std::string_view(name_) != other.name_;
    }

    constexpr bool operator<(const WClass &other) const noexcept
    {
        return std::string_view(name_) < other.name_;
    }
    
    constexpr bool operator>(const WClass &other) const noexcept
    {
        return std::string_view(name_) > other.name_;
    }

    constexpr bool operator<=(const WClass &other) const noexcept
    {
        return std::string_view(name_) <= other.name_;
    }
    
    constexpr bool operator>=(const WClass &other) const noexcept
    {
        return std::string_view(name_) >= other.name_;
    }

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

