#pragma once

#include <vector>
#include <memory>
#include <concepts>

class WImporter;

class WImportersRegister {
public:

    WImportersRegister()=default;
    ~WImportersRegister()=default;

    WImportersRegister(const WImportersRegister &)=delete;
    WImportersRegister(WImportersRegister && other) noexcept=default;

    WImportersRegister & operator=(const WImportersRegister &) = delete;
    WImportersRegister & operator=(WImportersRegister && other) noexcept = default;

    template<std::derived_from<WImporter> T, typename ... Args>
    void Register(Args&& ... args)
    {
        registry_.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    auto begin() {
        return registry_.rbegin();
    }

    auto end() {
        return registry_.rend();
    }

    auto begin() const {
        return registry_.crbegin();
    }

    auto end() const {
        return registry_.crend();
    }

    size_t Size() const noexcept
    {
        return registry_.size();
    }

    bool IsEmpty() const noexcept {
        return registry_.empty();
    }

private:

    std::vector<std::unique_ptr<WImporter>> registry_{};
    
};
