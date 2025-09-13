#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WCore/TWAllocator.hpp"
#include "WLog.hpp"

#include <memory>
#include <unordered_map>
#include <typeindex>

struct WDbBuilder {

public:

    template<typename B, typename I>
    using DbType = IObjectDataBase<B,I>;

    template<typename B, typename I>
    using DbRawPtr = DbType<B,I>*;

    using VoidPtr = void*;

    template<typename B, typename I>
    using RegKey = std::pair<B,I>;

    using Register = std::unordered_map<std::type_index, TFnPtr< VoidPtr() >>;

    template<typename T>
    struct ParamType{
        using Type=T;
    };

    constexpr WDbBuilder()=default;

    virtual ~WDbBuilder()=default;

    constexpr WDbBuilder(const WDbBuilder & other) :
        register_(other.register_) {}

    constexpr WDbBuilder(WDbBuilder && other) noexcept :
        register_(std::move(other.register_)) {}

    constexpr WDbBuilder & operator=(const WDbBuilder & other) {
        if (this != &other) {
            register_ = other.register_;
        }
        return *this;
    }

    constexpr WDbBuilder & operator=(WDbBuilder && other) {
        if (this != &other) {
            register_ = std::move(other.register_);
        }

        return *this;
    }

    template<typename B, typename I>
    std::unique_ptr<DbType<B,I>> Create() const {
        return std::unique_ptr<DbType<B,I>>(
            CastDb<B,I>(
                CreateDb(typeid(RegKey<B,I>)))
            );
    }

    template<typename T, CConvertibleTo<T> B, typename I>
    void RegisterBuilder() {
        if (register_.contains(typeid(RegKey<B,I>))) {
            return;
        }

        register_[typeid(RegKey<B,I>)] = &CreateObjectDb<T,B,I>;
    }

private:

    template<typename T, CConvertibleTo<T> B, typename I>
    static inline VoidPtr CreateObjectDb() {
        
        auto allocatefn = [] (T * _pptr, size_t _pn, T* _nptr, size_t _nn) {
            if (_pptr) {
                for(size_t i=0; i<_pn; i++) {
                    if (!BWRef::IsInstanced(_pptr + i)) continue;
                        
                    for (auto & ref : BWRef::Instances(_pptr + i)) {
                        if (ref == nullptr) continue;
                            
                        ref->BSet(_nptr + i);
                    }
                }
            }
        };

        auto deallocatefn = [] (T * _ptr, std::size_t _n) {
            if (_ptr) {
                for(std::size_t i=0; i<_n; i++) {
                    if (!BWRef::IsInstanced(_ptr + i)) continue;

                    for (auto & ref : BWRef::Instances(_ptr + i)) {
                        if (ref == nullptr) continue;

                        ref->BSet(_ptr + i);
                    }
                }
            }
        };

        TWAllocator<T, decltype(allocatefn), decltype(deallocatefn)> a;

        a.SetAllocateFn(allocatefn);
            // []
            // (T * _pptr, size_t _pn, T* _nptr, size_t _nn) {
            //     if (_pptr) {
            //         for(size_t i=0; i<_pn; i++) {
            //             if (!BWRef::IsInstanced(_pptr + i)) continue;
                        
            //             for (auto & ref : BWRef::Instances(_pptr + i)) {
            //                 if (ref == nullptr) continue;
                            
            //                 ref->BSet(_nptr + i);
            //             }
            //         }
            //     }
            // }
            // );

        a.SetDeallocateFn(deallocatefn);
            // []
            // (T * _ptr, std::size_t _n) {
            //     if (_ptr) {
            //         for(std::size_t i=0; i<_n; i++) {
            //             if (!BWRef::IsInstanced(_ptr + i)) continue;

            //             for (auto & ref : BWRef::Instances(_ptr + i)) {
            //                 if (ref == nullptr) continue;

            //                 ref->BSet(_ptr + i);
            //             }
            //         }
            //     }
            // }
            // );

        auto crtfn = [](const I & in_Id) -> T { return T{}; };
        auto dstrfn = [](T & in_obj) -> void {};

        return new TObjDb<T,
                          decltype(crtfn),
                          decltype(dstrfn),
                          B,
                          I,
                          decltype(a)>(crtfn, dstrfn, a);
    }

    VoidPtr CreateDb(std::type_index typeindex) const {
        assert(register_.contains(typeindex));
        
        return register_.at(typeindex)();
    }

    template<typename B, typename I>
    DbRawPtr<B,I> CastDb(VoidPtr in_cast) const {
        return static_cast<DbRawPtr<B,I>>(in_cast);
    }

    Register register_;

};
