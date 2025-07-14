#pragma once
#include "WEngineObjects/WObjectMacros.hpp"

_H()

_INCLUDE("cosa")


DECLARE_WCLASS(WObject)

// // #include "WCore/WCore.hpp"
// #include "WEngineObjects/WClass.hpp"
// #include "WCore/TObjectDataBase.hpp"

// class WObject;

// class WENGINEOBJECTS_API WClass__WObject : public WClass{
// public :

//     constexpr WClass__WObject() :
//         WClass("WClass__WObject") {}

//     ~WClass__WObject() override = default;

// public:

//     std::unique_ptr<IObjectDataBase> CreateObjectDatabase() override;

//     WObject * DefaultObject() const override;

// };
