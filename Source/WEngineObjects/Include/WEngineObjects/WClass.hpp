#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WEngineObjects/TWAllocator.hpp"

#include <memory>

class WObject;

class WENGINEOBJECTS_API WClass
{
public:

    constexpr WClass() noexcept = default;
    
    constexpr WClass(const char *name) noexcept :
    name_(name)
    {}

    virtual ~WClass() = default;

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

    const WClass & operator=(WClass && other) noexcept
    {
        if (this != &other) {
            name_ = std::move(other.name_);
        }

        return *this;
    }

public:

    virtual std::unique_ptr<IObjectDataBase<WObject>> CreateObjectDatabase()=0;

    virtual const WObject * DefaultObject() const=0;

public:

    constexpr const char *Name() const noexcept {
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

