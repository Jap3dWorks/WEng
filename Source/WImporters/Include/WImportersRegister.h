#pragma once

#include "WCore/WCore.h"
#include "WCore/TOptionalRef.h"

#include <utility>
#include <vector>
#include <memory>
#include <concepts>

class WImporter;

class WIMPORTERS_API WImportersRegister {
public:

    constexpr WImportersRegister() noexcept = default;
    ~WImportersRegister()=default;

    WImportersRegister(const WImportersRegister &)=delete;
    constexpr WImportersRegister(WImportersRegister && other) noexcept=default;

    WImportersRegister & operator=(const WImportersRegister &) = delete;
    constexpr WImportersRegister & operator=(WImportersRegister && other) noexcept = default;

    template<std::derived_from<WImporter> T, typename ... Args>
    void Register(Args&& ... args)
    {
        registry_.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    constexpr auto begin() noexcept {
        return registry_.rbegin();
    }

    constexpr auto end() noexcept {
        return registry_.rend();
    }

    constexpr auto begin() const noexcept {
        return registry_.crbegin();
    }

    constexpr auto end() const noexcept {
        return registry_.crend();
    }

    constexpr size_t Size() const noexcept
    {
        return registry_.size();
    }

    constexpr bool IsEmpty() const noexcept {
        return registry_.empty();
    }

    template<std::derived_from<WImporter> T>
    TOptionalRef<T> GetImporter()
    {
        for (auto& importer : *this) {
            if (T * derived = dinamyc_cast<T*>(importer.get())) {
                return *derived;
            }
        }

        return null_optional_ref;
    }

    TOptionalRef<WImporter> GetImporter(const char * in_extension);

private:

    std::vector<std::unique_ptr<WImporter>> registry_{};
    
};
