#pragma once

#include <utility>
#include <vulkan/vulkan_core.h>

namespace wvk::raii {
    
    template<typename T, typename CreatorType>
    class VkRAII {

    public:

        VkRAII() = default;
    
        template<typename ...Args>
        VkRAII(const CreatorType& creator , Args && ...args) :
            creator_(creator) {
            InitializeWithArgs(std::forward<Args>(args)...);
        }

        template<typename ...Args>
        VkRAII(CreatorType && creator , Args && ...args) :
            creator_(std::move(creator)) {
            InitializeWithArgs(std::forward<Args>(args)...);
        }

        VkRAII(const VkRAII&) = delete;
        VkRAII& operator=(const VkRAII&) = delete;

        VkRAII(VkRAII&& other) noexcept :
            creator_(std::move(other.creator_)),
            value_(other.value_)
            {
                other.value_ = VK_NULL_HANDLE;
            }
    
        VkRAII& operator=(VkRAII&& other) noexcept {
            if (this != &other) {
                Destroy();
                creator_ = std::move(other.creator_);
                value_ = other.value_;

                other.value_=VK_NULL_HANDLE;
            }
            return *this;
        }

    
        virtual ~VkRAII() {
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

}
