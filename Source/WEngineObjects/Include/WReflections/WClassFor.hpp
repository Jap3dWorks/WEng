#pragma once

#include "WEngineObjects/WClass.hpp"

/**
 * @brief WClass declaration for an WObject.
 */
template<typename T>
class WClassFor : public WClass {
public:
    WClassFor() : WClass(typeid(T).name()) {}
    ~WClassFor() override = default;

public:

    std::unique_ptr<IObjectDataBase> CreateObjectDatabase() override;
    const WObject * DefaultObject() const override;
    
};
