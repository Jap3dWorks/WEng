#pragma once

using WClassKeyType = const void *;

/**
   DEPRECATED
   Might not be reliable between different compile processes like .so files.
   Better use std::type_index
*/
template<typename T>
struct TClassKey {

    static constexpr WClassKeyType Value() {
        return &v_;
    }
    
private:
    
    static constexpr WClassKeyType v_{nullptr};
};

template<typename T>
constexpr WClassKeyType TClassKey_v=TClassKey<T>::Value();
