#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"
#include "WEngineObjects/TWAllocator.hpp"
#include "WCore/TObjectDataBase.hpp"

class WComponent;

class WENGINEOBJECTS_API WClass__WComponent : public WClass{
public :

    constexpr WClass__WComponent() :
        WClass("WClass__WComponent") {}

    ~WClass__WComponent() override = default;

public:

    std::unique_ptr<IObjectDataBase> CreateObjectDatabase() override {
        std::make_unique<TObjectDataBase<WComponent, TWAllocator<WComponent>>>(
            CreateAllocator<WComponent>()
            );
        
        return nullptr;
    }

};
