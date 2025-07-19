#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TOptionalRef.hpp"

#include <memory>
#include <vector>

class IImporter;

/**
 * @brief 
 */
class IImporterRegister {
public:

    virtual ~IImporterRegister()=default;

    template<typename T, typename ...Args>
    void Register(Args && ... args) {
        Register(std::make_unique<T>(std::forward<Args>(args)...));
    }

    virtual void Register(std::unique_ptr<IImporter> && in_importer)=0;

    /**
     * @brief Apply in_fn for each Importer registered.
     * If in_fn returns true, the process should continue,
     * if in_fn returns false, the process should stop.
     */
    virtual void ForEach(const TFunction<bool(IImporter*)> & in_fn) const;

    TOptionalRef<IImporter> GetImporter(const char * in_extension) const;
    
    template<std::derived_from<IImporter> T>
    TOptionalRef<T> GetImporter() const {
        T* r=nullptr;

        ForEach([&r](IImporter* _imp) -> bool {
            if (T * d = dynamic_cast<T*>(_imp)) {
                r=d;
                return false;
            } else {
                return true;
            }
        });

        if (r) {
            return r;
        } else {
            return null_optional_ref;
        }
    }

};

