#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WCore/TWAllocator.hpp"
#include "WObjectDb/WDbBuilder.hpp"
#include "WLog.hpp"

#include <memory>
#include <unordered_set>
#include <type_traits>
#include <typeindex>
#include <cassert>

class WObject;
class WEntity;
class WAsset;
class WLevel;
class WComponent;

class WENGINEOBJECTS_API WClass
{
public:

    constexpr WClass() noexcept = default;
    
    constexpr WClass(const char *name) noexcept :
    name_(name)
    {}

    constexpr ~WClass() = default;

    constexpr WClass(const WClass & other) noexcept :
    name_(other.name_)
    {}

    constexpr WClass(WClass && other) noexcept :
    name_(std::move(other.name_))
    {}

    constexpr WClass & operator=(const WClass & other) noexcept
    {
        if (this != &other) {
            name_ = other.name_;
        }

        return *this;
    }

    constexpr const WClass & operator=(WClass && other) noexcept
    {
        if (this != &other) {
            name_ = std::move(other.name_);
        }

        return *this;
    }

public:

    virtual WDbBuilder DbBuilder() const=0;
    
    virtual constexpr const WClass * BaseClass() const=0;

    virtual constexpr std::unordered_set<const WClass*> Bases() const =0;

    /**
     * Returns true if other is derived from this.
     */
    constexpr bool IsBaseOf(const WClass * other) const {
        return other->Bases().contains(this);
    }

public:

    constexpr std::string Name() const noexcept {
        return name_;
    }

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

protected:



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
            return std::hash<std::string>{}(wclass.Name());
        }

    };
}

