#pragma once

#include <memory>

namespace wcr::memory {
    
    struct UniqueVoidPtr {
    public:

        UniqueVoidPtr() = default;
        UniqueVoidPtr(const UniqueVoidPtr&) = delete;
        UniqueVoidPtr(UniqueVoidPtr&&) = default;
        UniqueVoidPtr& operator=(const UniqueVoidPtr&) = delete;
        UniqueVoidPtr& operator=(UniqueVoidPtr&&) = default;
        ~UniqueVoidPtr() = default;

        template<typename T, typename ...Args>
        static inline UniqueVoidPtr MakeUnique(void(*deleter)(void*), Args &&... args) {
            return UniqueVoidPtr(
                new T{std::forward<Args>(args)...},
                deleter
                );
        }

        void * get() const {return ptr.get(); }

    private:

        UniqueVoidPtr(void * ptr, void(*deleter)(void*)) :
            ptr(ptr, deleter)
            {}

        static inline void NoDelete(void*) {}

        std::unique_ptr<void, void(*)(void*)> ptr{nullptr, &NoDelete};

    };

};
