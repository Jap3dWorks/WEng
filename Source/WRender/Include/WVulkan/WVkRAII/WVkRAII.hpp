#pragma once

#include <utility>
#include <vulkan/vulkan_core.h>

template<typename T, typename CreatorType>
class WVkRAII {

public:

    WVkRAII() = default;
    
    template<typename ...Args>
    WVkRAII(const CreatorType& creator , Args && ...args) :
        creator_(creator) {
        InitializeWithArgs(std::forward<Args>(args)...);
    }

    template<typename ...Args>
    WVkRAII(CreatorType && creator , Args && ...args) :
        creator_(std::move(creator)) {
        InitializeWithArgs(std::forward<Args>(args)...);
    }

    WVkRAII(const WVkRAII&) = delete;
    WVkRAII& operator=(const WVkRAII&) = delete;

    WVkRAII(WVkRAII&& other) noexcept :
        creator_(std::move(other.creator_)),
        value_(other.value_)
        {
            other.value_ = VK_NULL_HANDLE;
        }
    
    WVkRAII& operator=(WVkRAII&& other) noexcept {
        if (this != &other) {
            Destroy();
            creator_ = std::move(other.creator_);
            value_ = other.value_;

            other.value_=VK_NULL_HANDLE;
        }
        return *this;
    }

    
    virtual ~WVkRAII() {
        Destroy();
    }

    T & operator*() noexcept { return value_; }
    T & Value() noexcept { return value_; }

    const T & operator*() const noexcept { return value_; }
    const T & Value() const noexcept { return value_; }

    CreatorType & Creator() { return creator_; }
    const CreatorType & Creator() const { return creator_; }

private:

    void Initialize() {
        value_ = creator_.Create();
    }

    template <typename ...Args>
    void InitializeWithArgs(Args && ...args){
        value_ = creator_.Create(std::forward<Args>(args)...);
    }

    void Destroy() {
        if (value_ != VK_NULL_HANDLE) {
            creator_.Destroy(value_);
            value_ = VK_NULL_HANDLE;
        }
    }

private:

    CreatorType creator_{};
    T value_ {VK_NULL_HANDLE};

};
