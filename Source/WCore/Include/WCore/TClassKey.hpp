#pragma once

using WClassKeyType = const void *;

template<typename T>
struct TClassKey {

    static constexpr WClassKeyType Value() {
        return &v_;
    }
    
private:
    
    static inline constexpr WClassKeyType v_{nullptr};
};

template<typename T>
inline constexpr WClassKeyType TClassKey_v=TClassKey<T>::Value();
