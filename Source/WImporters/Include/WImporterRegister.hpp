#pragma once

#include "WImporters.hpp"
#include "WCore/TClassKey.hpp"

#include <utility>
#include <memory>
#include <cassert>
#include <typeindex>

class WIMPORTERS_API WImporterRegister {
    
public:

    constexpr WImporterRegister() noexcept :
        reg_() {}
    
    virtual ~WImporterRegister()=default;
    
    WImporterRegister(const WImporterRegister & other) :
    reg_() {
        for(auto& p : other.reg_) {
            reg_.insert({p.first, p.second->Clone()});
        }
    }

    constexpr WImporterRegister(WImporterRegister && other) noexcept :
    reg_(std::move(other.reg_))
    {}

    WImporterRegister & operator=(const WImporterRegister & other) {
        if (this != &other) {
            for(auto& p : other.reg_) {
                reg_.insert({p.first, p.second->Clone()});
            }
        }
        return *this;
    }

    constexpr WImporterRegister & operator=(WImporterRegister && other) noexcept {
        if (this != &other) {
            reg_ = std::move(other.reg_);
        }

        return *this;
    }

    /**
     * @brief run in_fn for each registered importer.
     * Is in_fn returns true the iteration continues.
     * if in_fn returns false the iteration stops.
     */
    template<CCallable<bool, WImporter*> TFn>
    void ForEach(TFn && in_fn) const {
        for(auto& p : reg_) {
            if (!in_fn(p.second.get())) {
                break;
            }
        }
    }

    template<typename T>
    void Register() {
        assert(!reg_.contains( std::type_index(typeid(T)) ));
        reg_.insert({std::type_index(typeid(T)), std::make_unique<T>()});
    }

    template<typename T>
    T GetImporter() const {
        assert(reg_.contains( std::type_index(typeid(T)) ));

        return *static_cast<T*>(reg_.at(std::type_index(typeid(T))).get());
    }

    TOptionalRef<WImporter> GetImporter(const char * in_extension) const {
        WImporter* r=nullptr;

        ForEach([&r, &in_extension](WImporter* _imp) ->bool {
            for (const std::string & s : _imp->Extensions()) {
                if (in_extension == s) {
                    r=_imp;
                    return false;
                }
            }
            return true;
        });

        if (r) {
            return r;
        } else {
            return null_optional_ref;
        }
    }

private:

    // std::unordered_map<WClassKeyType, std::unique_ptr<WImporter>> reg_;
    std::unordered_map<std::type_index, std::unique_ptr<WImporter>> reg_;
    
};
