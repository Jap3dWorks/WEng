#pragma once

#include <cstdint>

namespace wcr::type_traits {
    template<typename T>
    struct TemplateParams {};

    /**
     * Extract array max size
     */
    template<template <typename, std::size_t> typename T, typename D, std::size_t N>
    struct TemplateParams <T<D, N>> {
        using ElementType = D;
        static constexpr std::size_t Size=N;
    };
    
    template<typename T>
    constexpr std::size_t MaxSize_v = TemplateParams<T>::Size;

    template<typename T>
    using ElementType_t = TemplateParams<T>::ElementType;

    

}
