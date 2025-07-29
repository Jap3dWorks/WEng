#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WEngineObjects/TWAllocator.hpp"

#include <memory>
#include <unordered_set>

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

    virtual std::unique_ptr<IObjectDataBase<WObject>> CreateObjectDatabase() const =0;

    template<typename WBase, typename WIdClass>
    std::unique_ptr<IObjectDataBase<WBase,WIdClass>> CC () const {
        // return data_base_builder<WBase, WIdClass>();

        // BDBBuilder<WBase, WIdClass> * b;
        // return b->Create();
        
    }

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

    virtual BBdr * CreateBd();

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


struct BBdr {
    virtual std::unique_ptr<void*> CreateRaw()=0;


    template<typename Base, typename I>
    std::unique_ptr<IObjectDataBase<Base, I>> Create() {
        
    }

};



template<typename T>
struct BDBBuilder : BBdr {

    

    template<typename WBase, typename WIdClass>
    std::unique_ptr<IObjectDataBase<WBase, WIdClass>> _CreateObjectDb() {

        TWAllocator<T> a;
        a.SetAllocateFn(
            []
            (T * _pptr, size_t _pn, T* _nptr, size_t _nn) {
                if (_pptr) {
                    for(size_t i=0; i<_pn; i++) {
                        if (!BWRef::IsInstanced(_pptr + i)) {
                            continue;
                        }
                        
                        for (auto & ref : BWRef::Instances(_pptr + i)) {
                            if (ref == nullptr) continue;
                            
                            ref->BSet(_nptr + i);
                        }
                    }
                }
            }
            );

        return std::make_unique<TObjectDataBase<T,
                                                WBase,
                                                WIdClass,
                                                TWAllocator<T>>>(a);
    }
};

template<typename T, typename WBase, typename WIdClass>
struct WDBBuilder : public BDBBuilder<WBase, WIdClass> {

    std::unique_ptr<void*> CreateRaw() {
        return CreateObjectDb();
    }

    std::unique_ptr<IObjectDataBase<WBase, WIdClass>> CreateObjectDb() override {
        TWAllocator<T> a;
        a.SetAllocateFn(
            []
            (T * _pptr, size_t _pn, T* _nptr, size_t _nn) {
                if (_pptr) {
                    for(size_t i=0; i<_pn; i++) {
                        if (!BWRef::IsInstanced(_pptr + i)) {
                            continue;
                        }
                        
                        for (auto & ref : BWRef::Instances(_pptr + i)) {
                            if (ref == nullptr) continue;
                            
                            ref->BSet(_nptr + i);
                        }
                    }
                }
            }
            );

        return std::make_unique<TObjectDataBase<T,
                                                WBase,
                                                WIdClass,
                                                TWAllocator<T>>>(a);
    }

};
